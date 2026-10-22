/*
 * main_uld.cpp
 *
 * Carga aerea en ULDs (3D Multiple Bin Size Bin Packing Problem con restricciones de
 * transporte, instancias de Paquay et al. 2018): los ULDs se cargan uno a uno con BSG,
 * eligiendo el siguiente ULD como en Hessler et al. (2024), hasta cargar todas las cajas.
 * Al final se intenta recargar las cajas del ultimo ULD en un ULD mas pequeno.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>
#include "args.hxx"
#include "clpState.h"
#include "VCS_Function.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "BSG.h"

using namespace std;
using namespace clp;

// ejemplo: BSG_ULD problems/clp/benchs/ULD/Final_instances/n50sample0.csv -t 1

/** resultado de cargar un ULD */
struct Load {
	const ULD* uld;
	vector<int> boxes;      //cajas cargadas de cada tipo
	double volume;          //volumen cargado
	double weight;
	double cog[3];
	bool feasible;
	double value;           //evaluacion de la carga (volumen penalizado por el CoG)
	vector<Block::PlacedBox> placed;  //cajas ubicadas

	double utilization() const { return volume/uld->volume; }
	bool cog_ok() const {
		return fabs(cog[0]-uld->L/2.0) <= uld->dev[0] && fabs(cog[1]-uld->W/2.0) <= uld->dev[1] &&
			cog[2] <= uld->dev[2];
	}
};

//lineas del archivo de la instancia (desde 0) con las cajas de cada tipo
static vector< vector<int> > type_items;

/** lee las cajas (formato de Paquay et al.) agrupando las identicas en tipos */
static vector<int> read_items(const string& file){
	ifstream in(file.c_str());
	if(!in){
		cerr << "No se pudo abrir el archivo " << file << endl;
		exit(1);
	}
	//cada linea: l;w;h;peso;l vertical;w vertical;h vertical;fragil;muy densa
	map<vector<double>, int> type;
	vector<int> n;
	string line;
	int item=0;
	while(getline(in, line)){
		vector<double> f;
		stringstream ss(line);
		string tok;
		while(getline(ss, tok, ';')) if(!tok.empty()) f.push_back(atof(tok.c_str()));
		if(f.size()<9) continue;
		f.resize(9);
		if(!type.count(f)){
			int t=clpState::types.size();
			type[f]=t;
			clpState::types.push_back(new BoxShape(t, t, (long) f[0], (long) f[1], (long) f[2],
					f[4]!=0, f[5]!=0, f[6]!=0, f[3], f[7]!=0));
			n.push_back(0);
			type_items.push_back(vector<int>());
		}
		n[type[f]]++;
		type_items[type[f]].push_back(item++);
	}
	return n;
}

/**
 * la caja cabe sola en el ULD u: sobre el piso dentro del borde o, si el ULD tiene borde,
 * sobre la subestructura (apoyada en ella al menos en min_overlap de su base)
 */
static bool fits(const BoxShape& box, const ULD& u){
	if(box.weight > u.max_weight) return false;
	long e=u.edge;
	for(int o=0; o<6; o++){
		if(!box.is_valid(o)) continue;
		long l=box.getL(o), w=box.getW(o), h=box.getH(o);
		if(l<=u.L-2*e && w<=u.W-2*e && h<=u.H){
			long x=e, z=0;
			if(u.adjust(x, z, l, h, e, u.L-e-l, 0, u.H-h, !clpState::support)) return true;
		}
		if(e>0 && l<=u.L && w<=u.W && h<=u.H-u.delta){
			long x=(u.L-l)/2, z=u.delta;
			if(!u.adjust(x, z, l, h, 0, u.L-l, u.delta, u.H-h, false)) continue;
			long y=(u.W-w)/2;
			double in = (double)(min(x+l, u.L-e)-max(x, e)) * (double)(min(y+w, u.W-e)-max(y, e));
			if(in >= max(clpState::min_overlap, 1e-9)*l*w || (x>=e && x+l<=u.L-e && y>=e && y+w<=u.W-e)) return true;
		}
	}
	return false;
}

//ULDs (sin y con subestructura) y cantidad disponible de cada tipo (-1: ilimitada)
static vector<ULD> ulds, ulds_sub;
static vector<int> available;
static bool use_substructure=false;

static int uld_index(const ULD& u){
	for(size_t i=0; i<ulds.size(); i++) if(ulds[i].type==u.type) return i;
	return -1;
}

/**
 * Siguiente ULD (Hessler et al. 2024): entre los ULDs en que caben las cajas que caben
 * en menos tipos de ULD, el de mayor volumen de cajas que caben en el (desempate: mayor volumen)
 */
static const ULD* select_uld(const vector<int>& left){
	int m=ulds.size()+1;
	vector< vector<bool> > F(clpState::types.size(), vector<bool>(ulds.size()));
	for(const BoxShape* b : clpState::types){
		if(left[b->type]==0) continue;
		int nf=0;
		for(size_t u=0; u<ulds.size(); u++) nf += (F[b->type][u] = (available[u]!=0 && fits(*b, ulds[u])));
		if(nf>0) m=min(m, nf);
	}

	const ULD* best=NULL;
	double best_vol=-1;
	for(size_t u=0; u<ulds.size(); u++){
		bool candidate=false;
		double vol=0;
		for(const BoxShape* b : clpState::types){
			if(left[b->type]==0 || !F[b->type][u]) continue;
			vol += (double) left[b->type]*b->getVolume();
			int nf=count(F[b->type].begin(), F[b->type].end(), true);
			if(nf==m) candidate=true;
		}
		if(!candidate) continue;
		if(vol>best_vol || (vol==best_vol && ulds[u].volume>best->volume)){ best=&ulds[u]; best_vol=vol; }
	}
	return best;
}

/**
 * Verificacion caja por caja: dentro del ULD (y sus recortes, fuera del borde bajo delta),
 * sin intersecciones, sin superar el peso maximo, cajas apoyadas (si hay soporte) y nada
 * sobre las fragiles
 */
static bool check_boxes(const clpState& s, const ULD& u){
	vector<Block::PlacedBox> B;
	for(const PlacedBlock& b : s.cont.blocks) b.block->get_boxes(b.xmin, b.ymin, b.zmin, B);

	double weight=0;
	for(size_t i=0; i<B.size(); i++){
		const Block::PlacedBox& a=B[i];
		AABB ai(a.xmin, a.ymin, a.zmin, a.xmax, a.ymax, a.zmax);
		weight += clpState::types[a.type]->weight;
		if(a.xmin<0 || a.ymin<0 || a.zmin<0 || a.xmax>u.L || a.ymax>u.W || a.zmax>u.H || !u.inside(ai)){
			cerr << "box outside the ULD " << ai << endl; return false;
		}
		if(a.zmin < u.delta && (u.substructure || a.xmin < u.edge || a.ymin < u.edge ||
				a.xmax > u.L-u.edge || a.ymax > u.W-u.edge)){
			cerr << "box on the edge of the ULD " << ai << endl; return false;
		}
		long supported=0;
		vector<clpState::Face> faces;
		if(u.substructure && a.zmin==u.delta)
			faces.push_back(clpState::Face{u.edge, u.edge, u.L-u.edge, u.W-u.edge, false, true});
		for(size_t j=0; j<B.size(); j++){
			if(j==i) continue;
			const Block::PlacedBox& b=B[j];
			AABB bj(b.xmin, b.ymin, b.zmin, b.xmax, b.ymax, b.zmax);
			if(j>i && ai.strict_intersects(bj)){ cerr << "boxes intersect " << ai << " " << bj << endl; return false; }
			if(b.zmax > a.zmin || b.zmax < a.zmin-clpState::padding) continue;
			bool fragile=clpState::types[b.type]->fragile;
			faces.push_back(clpState::Face{b.xmin, b.ymin, b.xmax, b.ymax, fragile, b.zmax==a.zmin});
			long dx=min(a.xmax, b.xmax)-max(a.xmin, b.xmin), dy=min(a.ymax, b.ymax)-max(a.ymin, b.ymin);
			if(b.zmax==a.zmin && dx>0 && dy>0){
				if(fragile){ cerr << "box on a fragile box " << ai << endl; return false; }
				supported += dx*dy;
			}
		}
		if(clpState::support && a.zmin>0){
			bool ok = clpState::corner_support? clpState::box_supported(a.xmin, a.ymin, a.xmax, a.ymax, faces) :
					supported==(a.xmax-a.xmin)*(a.ymax-a.ymin);
			if(!ok){ cerr << "box not supported " << ai << endl; return false; }
		}
	}
	if(weight > u.max_weight + 1e-6){ cerr << "weight exceeded" << endl; return false; }

	//cantidades de cajas
	vector<int> n(clpState::types.size(), 0);
	for(auto& b : B) n[b.type]++;
	for(size_t t=0; t<n.size(); t++)
		if(n[t] != clpState::initial_boxes[t]-s.left[t]){ cerr << "wrong number of boxes" << endl; return false; }
	return true;
}

struct Params {
	double alpha, beta, gamma, p, delta, min_fr, time;
	int max_bl;
	bool fix_knapsack;
};

/** carga el ULD u con las cajas left usando BSG (P.time segundos de busqueda) */
static Load load_bsg(const ULD& u, const vector<int>& left, const Params& P){
	Load res;
	res.uld=&u;
	res.boxes.assign(left.size(), 0);
	res.volume=res.weight=0.0;
	res.cog[0]=res.cog[1]=res.cog[2]=0.0;
	res.feasible=true;
	res.value=0.0;

	clpState* s0 = new_state(u, left, P.min_fr, P.max_bl);
	if(s0->get_n_valid_blocks()==0){ delete s0; return res; }

	VCS_Function* vcs = new VCS_Function(clpState::types, s0->left, s0->valid_blocks,
			u.L, u.W, u.H, P.alpha, P.beta, P.gamma, P.p, P.delta, P.fix_knapsack);
	Greedy* gr = new Greedy(vcs);
	BSG* bsg = new BSG(vcs, *gr, 4);
	DoubleEffort* de = new DoubleEffort(*bsg);

	//el tiempo limite es para la busqueda (sin la generacion de bloques)
	clock_t begin_time=clock();

	//la salida de la busqueda se descarta
	streambuf* buf=cout.rdbuf(NULL);
	State* s_copy = s0->clone();
	de->run(*s_copy, P.time, begin_time);
	cout.rdbuf(buf);

	clpState* best = de->get_best_state()? static_cast<clpState*>(de->get_best_state()->clone()) : NULL;
	if(best){
		//se desplaza la carga para centrar el centro de gravedad
		long dx, dy;
		best->cog_violation(dx, dy);
		best->translate(dx, dy);

		for(size_t t=0; t<left.size(); t++) res.boxes[t] = left[t] - best->left[t];
		res.volume = best->cont.getOccupiedVolume();
		res.weight = best->get_weight();
		best->get_cog(res.cog[0], res.cog[1], res.cog[2]);
		res.feasible = best->is_feasible() && check_boxes(*best, u);
		for(const PlacedBlock& b : best->cont.blocks) b.block->get_boxes(b.xmin, b.ymin, b.zmin, res.placed);
		res.value = best->get_value();
		delete best;
	}

	delete de; delete bsg; delete gr; delete vcs; delete s0;
	return res;
}

/**
 * carga el ULD u con las cajas left. Si el centro de gravedad no queda dentro del rango
 * permitido, se repite la busqueda anclando los espacios en el origen (la carga queda
 * agrupada y se puede centrar) y se elige la mejor de las dos cargas
 */
static Load load_cog(const ULD& u, const vector<int>& left, const Params& P){
	Load res = load_bsg(u, left, P);
	if(clpState::cog_penalty>0.0 && !res.cog_ok()){
		Space::origin_anchor=true;
		Load res2 = load_bsg(u, left, P);
		Space::origin_anchor=false;
		if(res2.value > res.value) res=res2;
	}
	return res;
}

/** carga el ULD del tipo de u, sin y (si hay borde) con subestructura, y elige la mejor carga */
static Load load(const ULD& u, const vector<int>& left, const Params& P){
	int i=uld_index(u);
	Load res = load_cog(ulds[i], left, P);
	if(use_substructure){
		Load res2 = load_cog(ulds_sub[i], left, P);
		if(res2.value > res.value) res=res2;
	}
	return res;
}

/**
 * Intenta eliminar un ULD: se liberan sus cajas y se recarga cada uno de los otros ULDs
 * (de mayor a menor volumen libre) con sus cajas y las liberadas; las que no se cargan
 * pasan al siguiente. El ULD se elimina si se cargan todas las cajas liberadas (los ULDs
 * recargados no pueden quedar con el centro de gravedad fuera de rango si no lo estaban).
 * Se prueban los ULDs de menor a mayor utilizacion. Retorna true si se elimino alguno.
 */
static bool eliminate_uld(vector<Load>& loads, const Params& P){
	if(loads.size()<2) return false;

	vector<int> order(loads.size());
	for(size_t i=0; i<order.size(); i++) order[i]=i;
	sort(order.begin(), order.end(), [&](int a, int b){ return loads[a].utilization() < loads[b].utilization(); });

	for(int target : order){
		vector<int> freed = loads[target].boxes;
		vector<Load> new_loads = loads;

		vector<int> others;
		for(size_t i=0; i<loads.size(); i++) if((int)i!=target) others.push_back(i);
		sort(others.begin(), others.end(), [&](int a, int b){
			return loads[a].uld->volume - loads[a].volume > loads[b].uld->volume - loads[b].volume; });

		for(int i : others){
			vector<int> avail = loads[i].boxes;
			for(size_t t=0; t<avail.size(); t++) avail[t]+=freed[t];
			Load ld = load(*loads[i].uld, avail, P);
			if(!ld.feasible || (loads[i].cog_ok() && !ld.cog_ok()) || ld.volume <= loads[i].volume) continue;
			new_loads[i]=ld;
			for(size_t t=0; t<freed.size(); t++) freed[t] = avail[t]-ld.boxes[t];
			if(count(freed.begin(), freed.end(), 0)==(int)freed.size()) break;
		}

		if(count(freed.begin(), freed.end(), 0)==(int)freed.size()){
			new_loads.erase(new_loads.begin()+target);
			loads=new_loads;
			return true;
		}
	}
	return false;
}

/**
 * Subconjunto de las cajas left para cargar el ULD u: las cajas que caben en u, de mayor a
 * menor volumen, hasta completar ratio veces el volumen del ULD (ratio<=0: todas)
 */
static vector<int> select_subset(const vector<int>& left, const ULD& u, double ratio){
	if(ratio<=0.0) return left;
	vector<int> types;
	for(const BoxShape* b : clpState::types)
		if(left[b->type]>0 && fits(*b, u)) types.push_back(b->type);
	sort(types.begin(), types.end(), [](int a, int b){
		return clpState::types[a]->getVolume() > clpState::types[b]->getVolume(); });

	vector<int> subset(left.size(), 0);
	double vol=0.0;
	for(int t : types){
		for(int k=0; k<left[t] && vol < ratio*u.volume; k++){
			subset[t]++;
			vol += clpState::types[t]->getVolume();
		}
		if(vol >= ratio*u.volume) break;
	}
	return subset;
}

/**
 * Cajas que no se cargaron (ULDs limitados): se recarga cada ULD (de mayor a menor volumen
 * libre) con sus cajas y las no cargadas, y se acepta si aumenta el volumen cargado
 * (sin dejar el centro de gravedad fuera de rango si no lo estaba)
 */
static void load_remaining(vector<Load>& loads, vector<int>& left, const Params& P){
	vector<int> order(loads.size());
	for(size_t i=0; i<order.size(); i++) order[i]=i;
	sort(order.begin(), order.end(), [&](int a, int b){
		return loads[a].uld->volume - loads[a].volume > loads[b].uld->volume - loads[b].volume; });
	for(int i : order){
		if(count(left.begin(), left.end(), 0)==(int)left.size()) return;
		vector<int> avail = loads[i].boxes;
		for(size_t t=0; t<avail.size(); t++) avail[t]+=left[t];
		Load ld = load(*loads[i].uld, avail, P);
		if(!ld.feasible || (loads[i].cog_ok() && !ld.cog_ok()) || ld.volume <= loads[i].volume) continue;
		loads[i]=ld;
		for(size_t t=0; t<left.size(); t++) left[t] = avail[t]-ld.boxes[t];
	}
}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-ULD *********.", "BSG Solver for air cargo loading (ULDs).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<string> _uld(parser, "string", "ULD file (default: uld.csv in the parent directory of the instance)", {"uld"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _max_bl(parser, "int", "Maximum number of blocks. Default: 1000", {"max_bl"});
	args::ValueFlag<double> _maxtime(parser, "double", "Timelimit of the search in each ULD (seconds, without the block generation). Default: 1", {'t', "timelimit"});
	args::ValueFlag<double> _cog_penalty(parser, "double", "Penalty of the deviation of the center of gravity (relative to the allowed one) in the evaluation of the solutions. Default: 10", {"cog_penalty"});
	args::ValueFlag<string> _support(parser, "string", "Support of the boxes: corners (the four bottom corners rest on non-fragile boxes or on the floor, as in Paquay et al. 2018), full (the whole base) or none. Default: corners", {"support"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter. Default: 2.4407", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter. Default: 2.2652", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter. Default: 0.3896", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter. Default: 0.0164", {'p'});
	args::Flag _fix_knapsack(parser, "fix_knapsack", "Volume loss: consider the lengths of all the allowed orientations of the boxes", {"fix_knapsack"});
	args::ValueFlag<double> _subset_ratio(parser, "double", "Each ULD is loaded with a subset of the remaining boxes (the largest ones) whose volume is ratio times the volume of the ULD (0: all the boxes). Default: 2", {"subset_ratio"});
	args::ValueFlag<long> _edge(parser, "int", "Width of the edge of the ULD floor on which loading is not allowed below the height of the vertical edge offset (Hessler et al. 2024). Default: 0", {"edge"});
	args::ValueFlag<long> _edge_offset(parser, "int", "Vertical edge offset (with an edge, a substructure of this height can also be used). Default: 0", {"edge_offset"});
	args::ValueFlag<double> _min_overlap(parser, "double", "Support: a box is also supported if part of its base rests on other boxes and the proportion min_overlap of its base rests on boxes or on padding (Hessler et al. 2024). Default: 0 (only the four corners)", {"min_overlap"});
	args::ValueFlag<long> _padding(parser, "int", "Maximum height of the padding material. Default: 0", {"padding"});
	args::ValueFlag<double> _cog_rel(parser, "double", "Maximum deviation of the center of gravity in x and y relative to the half dimensions of the ULD (Hessler et al. 2024; default: the deviations of the ULD file)", {"cog_rel"});
	args::ValueFlag<int> _uld_count(parser, "int", "Number of available ULDs of each type. Default: unlimited", {"uld_count"});
	args::Flag _no_eliminate(parser, "no_eliminate", "Do not try to eliminate ULDs by distributing their boxes into the other ULDs", {"no_eliminate"});
	args::ValueFlag<string> _output(parser, "string", "Write the solution to this file (one line per box: ULD, ULD type, substructure, item (line of the instance file, from 0), x, y, z, length, width, height)", {'o', "output"});
	args::Flag _verbose(parser, "verbose", "Print the loaded ULDs", {'v', "verbose"});
	args::Positional<std::string> _file(parser, "instance", "Instance file (format of Paquay et al.)");

	try{
		parser.ParseCLI(argc, argv);
	}catch (args::Help&){
		std::cout << parser;
		return 0;
	}catch (args::ParseError& e){
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}catch (args::ValidationError& e){
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	string file=_file.Get();
	string uld_file;
	if(_uld) uld_file=_uld.Get();
	else{
		string dir=file.substr(0, file.find_last_of('/')+1);
		uld_file=dir+"../uld.csv";
	}

	Params P;
	P.alpha=2.4407; P.beta=2.2652; P.gamma=0.3896; P.delta=1.0; P.p=0.0164;
	if(_alpha) P.alpha=_alpha.Get();
	if(_beta) P.beta=_beta.Get();
	if(_gamma) P.gamma=_gamma.Get();
	if(_delta) P.delta=_delta.Get();
	if(_p) P.p=_p.Get();
	P.min_fr=(_min_fr)? _min_fr.Get():0.98;
	P.time=(_maxtime)? _maxtime.Get():1.0;
	P.max_bl=(_max_bl)? _max_bl.Get():1000;
	P.fix_knapsack=_fix_knapsack;
	string support=(_support)? _support.Get() : "corners";
	if(support!="corners" && support!="full" && support!="none"){
		cerr << "Invalid support: " << support << endl;
		return 1;
	}
	clpState::support=(support!="none");
	clpState::corner_support=(support=="corners");
	clpState::cog_penalty=(_cog_penalty)? _cog_penalty.Get() : 10.0;

	double subset_ratio=(_subset_ratio)? _subset_ratio.Get() : 2.0;

	clpState::min_overlap=(_min_overlap)? _min_overlap.Get() : 0.0;
	clpState::padding=(_padding)? _padding.Get() : 0;

	clock_t begin_time=clock();

	ulds=ULD::read(uld_file);
	for(ULD& u : ulds){
		if(_edge) u.edge=_edge.Get();
		if(_edge_offset) u.delta=_edge_offset.Get();
		if(_cog_rel){
			u.dev[0]=(long) (_cog_rel.Get()*u.L/2.0);
			u.dev[1]=(long) (_cog_rel.Get()*u.W/2.0);
		}
	}
	use_substructure = clpState::corner_support && ulds[0].edge>0 && ulds[0].delta>0;
	ulds_sub=ulds;
	for(ULD& u : ulds_sub) u.substructure=true;
	available.assign(ulds.size(), (_uld_count)? _uld_count.Get() : -1);
	vector<int> left=read_items(file);

	for(const BoxShape* b : clpState::types){
		bool any=false;
		for(const ULD& u : ulds) any = any || fits(*b, u);
		if(!any && available[0]<0){
			cerr << "ERROR: box " << b->type << " does not fit in any ULD" << endl;
			return 1;
		}
	}

	vector<Load> loads;
	auto remaining=[&](){ int n=0; for(int x : left) n+=x; return n; };

	while(remaining()>0){
		const ULD* u = select_uld(left);
		//no quedan ULDs disponibles en que quepan las cajas restantes
		if(!u) break;
		Load ld = load(*u, select_subset(left, *u, subset_ratio), P);
		int n=0; for(int x : ld.boxes) n+=x;
		//si no se cargo ninguna caja del subconjunto, se usan todas
		if(n==0 && subset_ratio>0.0){
			ld = load(*u, left, P);
			for(int x : ld.boxes) n+=x;
		}
		if(n==0){
			cerr << "ERROR: no box was loaded in ULD " << u->type << endl;
			return 1;
		}
		for(size_t t=0; t<left.size(); t++) left[t]-=ld.boxes[t];
		loads.push_back(ld);
		if(available[uld_index(*u)]>0) available[uld_index(*u)]--;
	}

	if(remaining()>0) load_remaining(loads, left, P);

	//se eliminan ULDs repartiendo sus cajas en los otros
	if(!_no_eliminate)
		while(eliminate_uld(loads, P));

	//se intenta recargar las cajas del ULD menos utilizado en el ULD mas pequeno posible
	Load& worst = *min_element(loads.begin(), loads.end(),
			[](const Load& a, const Load& b){ return a.utilization() < b.utilization(); });
	vector<const ULD*> smaller;
	for(size_t i=0; i<ulds.size(); i++)
		if(ulds[i].volume < worst.uld->volume && available[i]!=0) smaller.push_back(&ulds[i]);
	sort(smaller.begin(), smaller.end(), [](const ULD* a, const ULD* b){ return a->volume < b->volume; });
	for(const ULD* u : smaller){
		const vector<int>& boxes=worst.boxes;
		bool all_fit=true;
		for(size_t t=0; t<boxes.size(); t++)
			if(boxes[t]>0 && !fits(*clpState::types[t], *u)) all_fit=false;
		if(!all_fit) continue;
		Load ld = load(*u, boxes, P);
		if(ld.boxes==boxes && (ld.cog_ok() || !worst.cog_ok())){
			if(available[uld_index(*u)]>0) available[uld_index(*u)]--;
			if(available[uld_index(*worst.uld)]>=0) available[uld_index(*worst.uld)]++;
			worst=ld;
			break;
		}
	}

	double time=double(clock()-begin_time)/double(CLOCKS_PER_SEC);

	//resumen
	vector<double> util;
	double total_vol=0, total_uld_vol=0, cost=0;
	int cog_viol=0;
	bool feasible=true;
	for(const Load& ld : loads){
		util.push_back(ld.utilization()*100);
		total_vol+=ld.volume;
		total_uld_vol+=ld.uld->volume;
		cost+=ld.uld->cost;
		cog_viol+=!ld.cog_ok();
		feasible = feasible && ld.feasible && ld.weight <= ld.uld->max_weight + 1e-6;
	}
	vector<double> su=util;
	sort(su.begin(), su.end());
	double median = (su.size()%2)? su[su.size()/2] : (su[su.size()/2-1]+su[su.size()/2])/2;

	cout.precision(6);
	if(_verbose){
		for(size_t i=0; i<loads.size(); i++){
			const Load& ld=loads[i];
			int n=0; for(int x : ld.boxes) n+=x;
			cout << "ULD " << i+1 << " type " << ld.uld->type << (ld.uld->substructure? " (substructure)":"") << ": boxes " << n
				<< " utilization " << util[i] << "% weight " << ld.weight << "/" << ld.uld->max_weight
				<< " cog (" << ld.cog[0]-ld.uld->L/2.0 << "," << ld.cog[1]-ld.uld->W/2.0 << ")"
				<< (ld.cog_ok()? "":" violated") << endl;
		}
	}
	int unloaded=remaining();
	cout << "n_ulds " << loads.size() << " median_util " << median
		<< " total_util " << 100*total_vol/total_uld_vol << " cost " << cost
		<< " cog_violations " << cog_viol << " unloaded " << unloaded << " time " << time << endl;

	if(_output){
		ofstream out(_output.Get().c_str());
		out << "uld;uld_type;substructure;item;x;y;z;l;w;h" << endl;
		vector<size_t> next(type_items.size(), 0);
		for(size_t i=0; i<loads.size(); i++)
			for(const Block::PlacedBox& b : loads[i].placed)
				out << i << ";" << loads[i].uld->type << ";" << loads[i].uld->substructure << ";" << type_items[b.type][next[b.type]++] << ";"
					<< b.xmin << ";" << b.ymin << ";" << b.zmin << ";"
					<< b.xmax-b.xmin << ";" << b.ymax-b.ymin << ";" << b.zmax-b.zmin << endl;
	}

	if(!feasible){
		cerr << "ERROR: infeasible solution" << endl;
		return 1;
	}
	return 0;
}
