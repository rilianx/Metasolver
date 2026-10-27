/*
 * main_bpp.cpp
 *
 * Problema de carga de multiples contenedores identicos (3D Bin Packing Problem, MCLP):
 * todas las cajas se deben cargar en el menor numero de contenedores L x W x H.
 *
 * Los contenedores se cargan uno a uno con BSG (como en BSG_ULD) y la solucion se mejora
 * vaciando contenedores: las cajas del contenedor menos cargado se insertan en los demas
 * con ruina y reconstruccion; si sobran cajas y caben en un contenedor, el cambio se acepta
 * igual (aumenta la suma de los cuadrados de los volumenes cargados, Fleszar y Hindi 2002),
 * lo que prepara la eliminacion del contenedor en los intentos siguientes.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>
#include <random>
#include <array>
#include <cmath>
#include "args.hxx"
#include "clpState.h"
#include "VCS_Function.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "BSG.h"

using namespace std;
using namespace clp;

// ejemplo: BSG_BPP problems/clp/benchs/MPV/c1_50_1.txt -t 0.1 -T 10

/** carga de un contenedor */
struct Load {
	vector<int> boxes;      //cajas cargadas de cada tipo
	double volume;          //volumen cargado
	bool feasible;
	vector<Block::PlacedBox> placed;  //cajas ubicadas
};

//contenedor (sin recortes, sin peso maximo)
static ULD bin;
//lineas del archivo de la instancia (desde 0) con las cajas de cada tipo
static vector< vector<int> > type_items;

/**
 * Lee la instancia: linea "n L W H" y luego una linea "l w h" por caja. Los archivos de las
 * instancias de Martello, Pisinger y Vigo (2000) tienen varias instancias, cada una precedida
 * por una linea de tres numeros (la primera es el numero de la instancia): se lee la
 * instancia inst (desde 1). Las cajas identicas se agrupan en tipos. Con rot, las cajas se
 * pueden rotar en las 6 orientaciones; si no, la orientacion es fija.
 */
static vector<int> read_instance(const string& file, bool rot, int inst){
	ifstream in(file.c_str());
	if(!in){
		cerr << "No se pudo abrir el archivo " << file << endl;
		exit(1);
	}
	int n=0;
	string line;
	while(getline(in, line)){
		stringstream ss(line);
		vector<long> f; long x;
		while(ss >> x) f.push_back(x);
		if(f.size()==4){ n=f[0]; bin.L=f[1]; bin.W=f[2]; bin.H=f[3]; break; }
		if(f.size()==3 && f[0]!=inst){
			//se salta la instancia: linea "n L W H" y n cajas
			getline(in, line);
			stringstream s2(line); int m; s2 >> m;
			for(int i=0; i<m; i++) getline(in, line);
		}
	}
	if(n==0){ cerr << "Instancia no encontrada" << endl; exit(1); }
	map<vector<long>, int> type;
	vector<int> left;
	for(int i=0; i<n; i++){
		vector<long> d(3);
		in >> d[0] >> d[1] >> d[2];
		if(!in){ cerr << "Instancia incompleta" << endl; exit(1); }
		if(!type.count(d)){
			int t=clpState::types.size();
			type[d]=t;
			clpState::types.push_back(new BoxShape(t, t, d[0], d[1], d[2], rot, rot, rot));
			left.push_back(0);
			type_items.push_back(vector<int>());
		}
		left[type[d]]++;
		type_items[type[d]].push_back(i);
	}
	return left;
}

static double volume_of(const vector<int>& b){
	double v=0;
	for(size_t t=0; t<b.size(); t++) v += (double) b[t]*clpState::types[t]->getVolume();
	return v;
}

static bool none(const vector<int>& b){ return count(b.begin(), b.end(), 0)==(int)b.size(); }

static int n_boxes(const vector<int>& b){ return accumulate(b.begin(), b.end(), 0); }

/** beneficio de las cajas b (volumen^kappa, ver --kappa) */
static double profit_of(const vector<int>& b){
	if(clpState::box_profit.empty()) return volume_of(b);
	double v=0;
	for(size_t t=0; t<b.size(); t++) v += (double) b[t]*clpState::box_profit[t];
	return v;
}

//criterio de aceptacion de la insercion: beneficio cargado (si no, volumen)
static bool accept_profit=true;

/** la carga ld (de las mismas cajas disponibles) es mejor que old */
static bool better(const Load& ld, const Load& old){
	if(!ld.feasible) return false;
	if(!accept_profit) return ld.volume > old.volume;
	double p1=profit_of(ld.boxes), p0=profit_of(old.boxes);
	return p1 > p0*(1+1e-12) || (p1 >= p0*(1-1e-12) && ld.volume > old.volume);
}

/**
 * Cota inferior del numero de contenedores (orientacion fija; con rotaciones, la cota
 * continua): maximo entre la cota continua y las cotas L1 y L2 de Martello, Pisinger y Vigo
 * (2000). L1: las cajas que no caben de a dos lado a lado en las dimensiones i y j se apilan en
 * la dimension k, lo que da un problema unidimensional acotado con la cota de Martello y Toth.
 * L2: a L1 se agrega el volumen de las cajas grandes y pequenas que no cabe en el espacio
 * libre de los contenedores de L1.
 */
static int lower_bound(const vector<int>& left){
	double V=(double) bin.L*bin.W*bin.H;
	int lb = (int) ceil(volume_of(left)/V - 1e-9);
	vector< array<long,3> > items;
	for(const BoxShape* b : clpState::types){
		if(!left[b->type]) continue;
		for(int o=1; o<6; o++) if(b->is_valid(o)) return lb;
		for(int c=0; c<left[b->type]; c++) items.push_back({b->getL(0), b->getW(0), b->getH(0)});
	}
	long D[3]={bin.L, bin.W, bin.H};
	for(int k=0; k<3; k++){
		//dimension k: apilamiento; dimensiones i, j: las cajas no caben de a dos
		int i=(k+1)%3, j=(k+2)%3;
		vector<long> d;
		for(auto& s : items) if(2*s[i] > D[i] && 2*s[j] > D[j]) d.push_back(s[k]);
		long C=D[k];
		long l1=0;
		for(long p=0; p<=C/2 && !d.empty(); p++){
			//J1: d > C-p; J2: C-p >= d > C/2; J3: C/2 >= d >= p
			long n1=0, n2=0, s3=0, free2=0;
			for(long x : d){
				if(x > C-p) n1++;
				else if(2*x > C){ n2++; free2+=C-x; }
				else if(x >= p) s3+=x;
			}
			long extra = max(0L, (long) ceil((double)(s3 - free2)/C - 1e-9));
			l1 = max(l1, n1+n2+extra);
		}
		lb = max(lb, (int) l1);
		//L2: p en la dimension i, q en la dimension j
		vector<long> P{1}, Q{1};
		for(auto& s : items){
			if(2*s[i] <= D[i]) P.push_back(s[i]);
			if(2*s[j] <= D[j]) Q.push_back(s[j]);
		}
		sort(P.begin(), P.end()); P.erase(unique(P.begin(), P.end()), P.end());
		sort(Q.begin(), Q.end()); Q.erase(unique(Q.begin(), Q.end()), Q.end());
		for(long p : P) for(long q : Q){
			double vol=0, dv=0;
			for(auto& s : items){
				if(s[i] > D[i]-p && s[j] > D[j]-q) dv += s[k];
				else if((2*s[i] > D[i] && 2*s[j] > D[j]) || (s[i] >= p && s[j] >= q))
					vol += (double) s[0]*s[1]*s[2];
			}
			double free = ((double) C*l1 - dv)*D[i]*D[j];
			lb = max(lb, (int)(l1 + max(0L, (long) ceil((vol - free)/V - 1e-9))));
		}
	}
	return lb;
}

struct Params {
	double alpha, beta, gamma, p, delta, min_fr, time;
	int max_bl;
	bool fix_knapsack;
};

static long st_bsg=0;

/**
 * carga un contenedor con las cajas left usando BSG (P.time segundos de busqueda). Las cajas
 * de fixed (si no es NULL) se ubican antes en sus posiciones
 */
static Load load_bsg(const vector<int>& left, const Params& P, const vector<Block::PlacedBox>* fixed=NULL){
	Load res;
	res.boxes.assign(left.size(), 0);
	res.volume=0.0;
	res.feasible=true;
	st_bsg++;

	clpState* s0 = new_state(bin, left, P.min_fr, P.max_bl);
	if(s0->get_n_valid_blocks()==0){ delete s0; return res; }

	if(fixed){
		for(const Block::PlacedBox& p : *fixed){
			const Block* blk=NULL;
			for(const Block* b : s0->valid_blocks)
				if(b->child[0]<0 && b->box_type==p.type && b->getL()==p.xmax-p.xmin &&
						b->getW()==p.ymax-p.ymin && b->getH()==p.zmax-p.zmin){ blk=b; break; }
			if(!blk){ cerr << "ERROR: fixed box not found" << endl; exit(1); }
			AABB a(p.xmin, p.ymin, p.zmin, p.xmax, p.ymax, p.zmax);
			s0->transition(clpAction(*blk, Space(a, bin.L, bin.W, bin.H), p.xmin, p.ymin, p.zmin));
		}
	}

	VCS_Function* vcs = new VCS_Function(clpState::types, s0->left, s0->valid_blocks,
			bin.L, bin.W, bin.H, P.alpha, P.beta, P.gamma, P.p, P.delta, P.fix_knapsack);
	Greedy* gr = new Greedy(vcs);
	BSG* bsg = new BSG(vcs, *gr, 4);
	DoubleEffort* de = new DoubleEffort(*bsg);

	clock_t begin_time=clock();
	streambuf* buf=cout.rdbuf(NULL);
	State* s_copy = s0->clone();
	de->run(*s_copy, P.time, begin_time);
	cout.rdbuf(buf);

	clpState* best = de->get_best_state()? static_cast<clpState*>(de->get_best_state()->clone()) : NULL;
	if(best){
		for(size_t t=0; t<left.size(); t++) res.boxes[t] = left[t] - best->left[t];
		res.volume = best->cont.getOccupiedVolume();
		res.feasible = best->is_feasible();
		for(const PlacedBlock& b : best->cont.blocks) b.block->get_boxes(b.xmin, b.ymin, b.zmin, res.placed);
		delete best;
	}
	delete de; delete bsg; delete gr; delete vcs; delete s0;
	return res;
}

/** continua la carga base (con sus cajas fijas en sus posiciones) agregando cajas de extra */
static Load load_more(const Load& base, const vector<int>& extra, const Params& P){
	vector<int> avail = base.boxes;
	for(size_t t=0; t<avail.size(); t++) avail[t]+=extra[t];
	return load_bsg(avail, P, &base.placed);
}

/** verificacion caja por caja: dentro del contenedor, sin intersecciones y cantidades correctas */
static bool check(const vector<Load>& loads, const vector<int>& total){
	vector<int> n(total.size(), 0);
	for(const Load& ld : loads){
		const vector<Block::PlacedBox>& B=ld.placed;
		for(size_t i=0; i<B.size(); i++){
			const Block::PlacedBox& a=B[i];
			n[a.type]++;
			if(a.xmin<0 || a.ymin<0 || a.zmin<0 || a.xmax>bin.L || a.ymax>bin.W || a.zmax>bin.H){
				cerr << "box outside the bin" << endl; return false;
			}
			const BoxShape& s=*clpState::types[a.type];
			bool ok=false;
			for(int o=0; o<6 && !ok; o++)
				ok = s.is_valid(o) && s.getL(o)==a.xmax-a.xmin && s.getW(o)==a.ymax-a.ymin && s.getH(o)==a.zmax-a.zmin;
			if(!ok){ cerr << "invalid orientation" << endl; return false; }
			AABB ai(a.xmin, a.ymin, a.zmin, a.xmax, a.ymax, a.zmax);
			for(size_t j=i+1; j<B.size(); j++){
				AABB bj(B[j].xmin, B[j].ymin, B[j].zmin, B[j].xmax, B[j].ymax, B[j].zmax);
				if(ai.strict_intersects(bj)){ cerr << "boxes intersect" << endl; return false; }
			}
		}
	}
	if(n!=total){ cerr << "wrong number of boxes" << endl; return false; }
	return true;
}

/**
 * Saca de la carga ld cajas que no tienen nada encima, de arriba hacia abajo, hasta sacar
 * la proporcion p del volumen cargado (el resto de la carga sigue siendo factible)
 */
static mt19937 rng(1);
static bool random_ruin=false;

static Load ruin(const Load& ld, double p){
	const vector<Block::PlacedBox>& B=ld.placed;
	vector<bool> gone(B.size(), false);
	double removed=0;
	while(removed < p*ld.volume){
		int best=-1, n_free=0;
		for(size_t i=0; i<B.size(); i++){
			if(gone[i]) continue;
			bool covered=false;
			for(size_t j=0; j<B.size() && !covered; j++){
				if(gone[j] || j==i) continue;
				if(B[j].zmin >= B[i].zmax &&
						min(B[i].xmax, B[j].xmax) > max(B[i].xmin, B[j].xmin) &&
						min(B[i].ymax, B[j].ymax) > max(B[i].ymin, B[j].ymin)) covered=true;
			}
			if(covered) continue;
			//aleatoria: una caja al azar entre las que no tienen nada encima
			if(random_ruin){ if(uniform_int_distribution<int>(0, n_free++)(rng)==0) best=i; }
			else if(best<0 || B[i].zmax > B[best].zmax) best=i;
		}
		if(best<0) break;
		gone[best]=true;
		removed += (double)(B[best].xmax-B[best].xmin)*(B[best].ymax-B[best].ymin)*(B[best].zmax-B[best].zmin);
	}

	Load res=ld;
	res.placed.clear();
	res.boxes.assign(ld.boxes.size(), 0);
	res.volume=0;
	for(size_t i=0; i<B.size(); i++){
		if(gone[i]) continue;
		res.placed.push_back(B[i]);
		res.boxes[B[i].type]++;
		res.volume += (double)(B[i].xmax-B[i].xmin)*(B[i].ymax-B[i].ymin)*(B[i].zmax-B[i].zmin);
	}
	return res;
}

//contadores (linea "stats")
static long st_swo=0, st_elim=0, st_partial=0, st_tries=0, st_ruin=0, st_scratch=0;
//etapas de la insercion
static bool stage_scratch=false, stage_ruin=true;
static vector<double> ruin_rhos={0.3, 0.6, 0.9};

/** contenedores de mayor a menor volumen libre */
static vector<int> by_free_volume(const vector<Load>& loads){
	vector<int> order(loads.size());
	iota(order.begin(), order.end(), 0);
	stable_sort(order.begin(), order.end(), [&](int a, int b){ return loads[a].volume < loads[b].volume; });
	return order;
}

/**
 * Inserta las cajas de pool en los contenedores cargados (sin agregar contenedores): para
 * cada proporcion rho, se saca rho del volumen de cada contenedor (ver ruin) y se reconstruye
 * (de mayor a menor volumen libre) dejando fijas las cajas que quedaron y agregando las del
 * pool. El cambio de cada contenedor se acepta si carga mas volumen. Con --scratch, antes se
 * recarga cada contenedor desde cero con sus cajas y las del pool.
 * pool queda con las cajas que no se insertaron.
 */
static void insert(vector<Load>& loads, vector<int>& pool, const Params& P){
	auto take=[&](int i, const Load& ld, const vector<int>& avail){
		loads[i]=ld;
		for(size_t t=0; t<pool.size(); t++) pool[t]=avail[t]-ld.boxes[t];
	};

	if(stage_scratch)
		for(int i : by_free_volume(loads)){
			if(none(pool)) return;
			vector<int> avail=loads[i].boxes;
			for(size_t t=0; t<avail.size(); t++) avail[t]+=pool[t];
			Load ld=load_bsg(avail, P);
			if(better(ld, loads[i])){ take(i, ld, avail); st_scratch++; }
		}

	vector<double> rhos=ruin_rhos;
	if(random_ruin) rhos={uniform_real_distribution<double>(0.2, 0.9)(rng)};
	if(stage_ruin)
		for(double rho : rhos){
			if(none(pool)) return;
			for(int i : by_free_volume(loads)){
				if(none(pool)) return;
				Load base = ruin(loads[i], rho);
				vector<int> avail=pool;
				for(size_t t=0; t<pool.size(); t++) avail[t] += loads[i].boxes[t]-base.boxes[t];
				Load ld = load_more(base, avail, P);
				if(!better(ld, loads[i])) continue;
				vector<int> all=avail;
				for(size_t t=0; t<all.size(); t++) all[t]+=base.boxes[t];
				take(i, ld, all);
				st_ruin++;
			}
		}
}

/** suma de los cuadrados de los volumenes cargados (relativos al contenedor) */
static double sum_squares(const vector<Load>& loads){
	double V=(double) bin.L*bin.W*bin.H, s=0;
	for(const Load& ld : loads) s += (ld.volume/V)*(ld.volume/V);
	return s;
}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-BPP *********.", "BSG Solver for the 3D bin packing problem (identical bins).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion). Default: 0.98", {"min_fr"});
	args::ValueFlag<int> _max_bl(parser, "int", "Maximum number of blocks. Default: 500", {"max_bl"});
	args::ValueFlag<double> _maxtime(parser, "double", "Timelimit of the search in each bin in the construction (seconds, without the block generation). Default: 0.1", {'t', "timelimit"});
	args::ValueFlag<double> _improve_time(parser, "double", "Timelimit of BSG in the improvement phase, relative to -t. Default: 1", {"improve_time"});
	args::ValueFlag<double> _total_time(parser, "double", "Total timelimit (seconds). Default: 60", {'T', "total_time"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter. Default: 2.4407", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter. Default: 2.2652", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter. Default: 0.3896", {"gamma"});
	args::ValueFlag<double> _p(parser, "double", "p parameter. Default: 0.0164", {'p'});
	args::ValueFlag<double> _kappa(parser, "double", "Profit of each box: volume^kappa (relative to the bin), to load the large boxes first. Default: 2", {"kappa"});
	args::Flag _accept_volume(parser, "accept_volume", "Insertion: accept the reload of a bin if it loads more volume (default: more profit)", {"accept_volume"});
	args::Flag _swo(parser, "swo", "Squeaky wheel optimization: repeat the construction increasing the profit of the boxes of the least loaded bins", {"swo"});
	args::ValueFlag<double> _swo_frac(parser, "double", "Proportion of the total time for the squeaky wheel optimization. Default: 0.5", {"swo_frac"});
	args::ValueFlag<double> _swo_rate(parser, "double", "Increase of the profit of the boxes of the least loaded bins. Default: 0.3", {"swo_rate"});
	args::ValueFlag<int> _inst(parser, "int", "Instance in files with several instances (from 1). Default: 1", {'i'});
	args::ValueFlag<string> _columns(parser, "string", "Write the bins generated in the construction and the squeaky wheel optimization (boxes of each type) to this file", {"columns"});
	args::Flag _rot(parser, "rot", "The boxes can be rotated (6 orientations). Default: fixed orientation", {"rot"});
	args::Flag _corner_anchor(parser, "corner_anchor", "Anchor each space at its closest corner of the bin (default: at the origin)", {"corner_anchor"});
	args::Flag _scratch(parser, "scratch", "Insertion of boxes also with a reload from scratch of each bin", {"scratch"});
	args::Flag _no_ruin(parser, "no_ruin", "Ablation: insertion of boxes without ruin and recreate", {"no_ruin"});
	args::Flag _no_partial(parser, "no_partial", "Ablation: only accept the elimination of a bin (not the partial transfers that increase the sum of squares)", {"no_partial"});
	args::Flag _no_improve(parser, "no_improve", "Ablation: only the sequential construction", {"no_improve"});
	args::ValueFlag<string> _output(parser, "string", "Write the solution to this file (one line per box: bin, item (from 0), x, y, z, l, w, h)", {'o', "output"});
	args::Flag _verbose(parser, "verbose", "Print the bins", {'v', "verbose"});
	args::Positional<std::string> _file(parser, "instance", "Instance file (n L W H, then l w h of each box)");

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

	Params P;
	P.alpha=(_alpha)? _alpha.Get() : 2.4407;
	P.beta=(_beta)? _beta.Get() : 2.2652;
	P.gamma=(_gamma)? _gamma.Get() : 0.3896;
	P.p=(_p)? _p.Get() : 0.0164;
	P.delta=1.0;
	P.min_fr=(_min_fr)? _min_fr.Get() : 0.98;
	P.time=(_maxtime)? _maxtime.Get() : 0.1;
	P.max_bl=(_max_bl)? _max_bl.Get() : 500;
	P.fix_knapsack=true;
	double total_time=(_total_time)? _total_time.Get() : 60.0;
	Params Pi=P; Pi.time=P.time*((_improve_time)? _improve_time.Get() : 1.0);

	clpState::support=false;
	clpState::cog_penalty=0.0;
	clpState::cog_greedy=0.0;
	clpState::translate_cog=false;
	Space::origin_anchor=!_corner_anchor;
	accept_profit=!_accept_volume;
	stage_scratch=_scratch; stage_ruin=!_no_ruin;

	clock_t begin_time=clock();
	auto elapsed=[&](){ return double(clock()-begin_time)/CLOCKS_PER_SEC; };

	vector<int> left=read_instance(_file.Get(), _rot, (_inst)? _inst.Get() : 1);
	const vector<int> total=left;
	bin.type=0; bin.max_weight=HUGE_VAL; bin.cost=1;
	bin.dev[0]=bin.L; bin.dev[1]=bin.W; bin.dev[2]=bin.H;
	bin.volume=(double) bin.L*bin.W*bin.H;
	int lb=lower_bound(left);
	double kappa=(_kappa)? _kappa.Get() : 2.0;
	vector<double> base_profit;
	for(const BoxShape* b : clpState::types)
		base_profit.push_back(bin.volume*pow(b->getVolume()/bin.volume, kappa));
	if(kappa!=1.0 || _swo) clpState::box_profit=base_profit;

	//construccion secuencial
	auto construct=[&](){
		vector<int> rest=total;
		vector<Load> res;
		while(!none(rest)){
			Load ld=load_bsg(rest, P);
			if(none(ld.boxes)){ cerr << "ERROR: a box does not fit in the bin" << endl; exit(1); }
			for(size_t t=0; t<rest.size(); t++) rest[t]-=ld.boxes[t];
			res.push_back(ld);
		}
		return res;
	};
	//menos contenedores y, a igual numero, el contenedor menos cargado con menos volumen
	auto min_vol=[&](const vector<Load>& L){ double m=HUGE_VAL; for(auto& l : L) m=min(m, l.volume); return m; };
	auto better_sol=[&](const vector<Load>& a, const vector<Load>& b){
		return a.size()<b.size() || (a.size()==b.size() && min_vol(a) < min_vol(b)); };

	vector<Load> loads=construct();
	int n_constr=loads.size();
	//contenedores generados (para --columns)
	vector< vector<int> > columns;
	for(const Load& l : loads) columns.push_back(l.boxes);

	//squeaky wheel optimization (Joslin y Clements 1999): se repite la construccion aumentando
	//el beneficio de los tipos de caja de los contenedores menos cargados
	if(_swo){
		double swo_time=total_time*((_swo_frac)? _swo_frac.Get() : 0.5);
		double rate=(_swo_rate)? _swo_rate.Get() : 0.3;
		vector<double> w(total.size(), 1.0);
		vector<Load> cur=loads;
		normal_distribution<double> noise(0.0, 0.05);
		while((int)loads.size()>lb && elapsed()<swo_time){
			vector<int> order=by_free_volume(cur);
			int m=max(1, (int) round(0.1*cur.size()));
			vector<bool> hit(total.size(), false);
			for(int k=0; k<m; k++)
				for(size_t t=0; t<total.size(); t++) if(cur[order[k]].boxes[t]>0) hit[t]=true;
			for(size_t t=0; t<total.size(); t++){
				if(hit[t]) w[t]*=1.0+rate;
				w[t]*=exp(noise(rng));
				clpState::box_profit[t]=base_profit[t]*w[t];
			}
			cur=construct();
			st_swo++;
			for(const Load& l : cur) columns.push_back(l.boxes);
			if(better_sol(cur, loads)){
				loads=cur;
				if(_verbose) cout << "swo " << elapsed() << " bins " << loads.size() << endl;
			}
		}
		clpState::box_profit=base_profit;
	}
	int n_swo=loads.size();
	if(_columns){
		ofstream out(_columns.Get().c_str());
		out << total.size() << " " << lb;
		for(int x : total) out << " " << x;
		out << endl;
		for(auto& c : columns){ for(int x : c) out << x << " "; out << endl; }
	}
	double t_constr=elapsed();

	//mejora: se vacian los contenedores, del menos al mas cargado. Cuando una pasada completa no
	//cambia la solucion, la ruina pasa a ser aleatoria (cajas y proporcion al azar)
	int fails=0;
	while(!_no_improve && (int)loads.size()>lb && elapsed()<total_time){
		bool improved=false;
		random_ruin = (fails>0);
		vector<int> order=by_free_volume(loads);
		for(int target : order){
			if(elapsed()>=total_time) break;
			st_tries++;
			vector<int> pool=loads[target].boxes;
			vector<Load> others;
			for(size_t i=0; i<loads.size(); i++) if((int)i!=target) others.push_back(loads[i]);
			double dbg0=volume_of(pool);
			insert(others, pool, Pi);
			if(getenv("BPP_DEBUG")){ double so=0; for(auto& o:others) so+=o.volume; cerr << "before " << dbg0/bin.volume << " after " << volume_of(pool)/bin.volume << " others " << so/bin.volume << endl; }
			if(none(pool)){
				loads=others; st_elim++; improved=true;
				if(_verbose) cout << "elim " << elapsed() << " bins " << loads.size() << endl;
				break;
			}
			//cambio parcial: las cajas que quedan se cargan en un contenedor (si caben todas) y se
			//acepta si aumenta la suma de los cuadrados de los volumenes cargados
			if(_no_partial) continue;
			Load fake; fake.volume=volume_of(pool);
			if(getenv("BPP_DEBUG")) cerr << "target " << loads[target].volume/bin.volume << " pool " << fake.volume/bin.volume << endl;
			others.push_back(fake);
			if(fake.volume > bin.volume || sum_squares(others) <= sum_squares(loads) + 1e-9) continue;
			Load ld=load_bsg(pool, Pi);
			if(!ld.feasible || ld.boxes!=pool) continue;
			others.back()=ld;
			loads=others; st_partial++; improved=true;
			break;
		}
		fails = improved? 0 : fails+1;
		//sin ruina aleatoria no hay mas cambios posibles
		if(!improved && !stage_ruin) break;
	}

	double time=elapsed();
	bool feasible=check(loads, total);
	for(const Load& ld : loads) feasible = feasible && ld.feasible;

	if(_verbose)
		for(size_t i=0; i<loads.size(); i++)
			cout << "bin " << i+1 << ": boxes " << n_boxes(loads[i].boxes) << " utilization "
				<< 100*loads[i].volume/bin.volume << "%" << endl;

	cout << "n_bins " << loads.size() << " lb " << lb << " construction " << n_constr << " swo " << n_swo
		<< " t_construction " << t_constr << " time " << time << endl;
	cout << "stats swo " << st_swo << " elim " << st_elim << " partial " << st_partial << " tries " << st_tries
		<< " ruin " << st_ruin << " scratch " << st_scratch << " bsg_calls " << st_bsg << endl;

	if(_output){
		ofstream out(_output.Get().c_str());
		out << "bin;item;x;y;z;l;w;h" << endl;
		vector<size_t> next(type_items.size(), 0);
		for(size_t i=0; i<loads.size(); i++)
			for(const Block::PlacedBox& b : loads[i].placed)
				out << i << ";" << type_items[b.type][next[b.type]++] << ";"
					<< b.xmin << ";" << b.ymin << ";" << b.zmin << ";"
					<< b.xmax-b.xmin << ";" << b.ymax-b.ymin << ";" << b.zmax-b.zmin << endl;
	}

	if(!feasible){
		cerr << "ERROR: infeasible solution" << endl;
		return 1;
	}
	return 0;
}
