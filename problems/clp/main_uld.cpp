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
#include <functional>
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
//todos los espacios anclados en el origen (la carga queda agrupada y se puede centrar)
static bool origin_only=true;
//ablacion (con --corner_anchor): recarga con los espacios anclados en el origen (ver load_cog)
static bool regroup=true;

static int uld_index(const ULD& u){
	for(size_t i=0; i<ulds.size(); i++) if(ulds[i].type==u.type) return i;
	return -1;
}

/**
 * Siguiente ULD (Hessler et al. 2024): entre los ULDs en que caben las cajas que caben
 * en menos tipos de ULD, el de mayor volumen de cajas que caben en el (desempate: mayor volumen)
 */
static const ULD* select_uld(const vector<int>& left, vector<const ULD*>* cands=NULL){
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
		if(cands) cands->push_back(&ulds[u]);
		if(vol>best_vol || (vol==best_vol && ulds[u].volume>best->volume)){ best=&ulds[u]; best_vol=vol; }
	}
	return best;
}

/**
 * Verificacion caja por caja: dentro del ULD (y sus recortes, fuera del borde bajo delta),
 * sin intersecciones, sin superar el peso maximo, cajas apoyadas (si hay soporte) y nada
 * sobre las fragiles
 */
static bool check_placed(const vector<Block::PlacedBox>& B, const ULD& u){
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
	return true;
}

static bool check_boxes(const clpState& s, const ULD& u){
	vector<Block::PlacedBox> B;
	for(const PlacedBlock& b : s.cont.blocks) b.block->get_boxes(b.xmin, b.ymin, b.zmin, B);
	if(!check_placed(B, u)) return false;

	//cantidades de cajas
	vector<int> n(clpState::types.size(), 0);
	for(auto& b : B) n[b.type]++;
	for(size_t t=0; t<n.size(); t++)
		if(n[t] != clpState::initial_boxes[t]-s.left[t]){ cerr << "wrong number of boxes" << endl; return false; }
	return true;
}

static bool none(const vector<int>& b);
static double volume_of(const vector<int>& b);

struct Params {
	double alpha, beta, gamma, p, delta, min_fr, time;
	int max_bl;
	bool fix_knapsack;
};

/** carga el ULD u con las cajas left usando BSG (P.time segundos de busqueda) */
static Load load_bsg(const ULD& u, const vector<int>& left, const Params& P,
		const vector<Block::PlacedBox>* fixed=NULL, bool center=true){
	Load res;
	res.uld=&u;
	res.boxes.assign(left.size(), 0);
	res.volume=res.weight=0.0;
	res.cog[0]=res.cog[1]=res.cog[2]=0.0;
	res.feasible=true;
	res.value=0.0;

	clpState* s0 = new_state(u, left, P.min_fr, P.max_bl);
	if(s0->get_n_valid_blocks()==0){ delete s0; return res; }

	//cajas fijas: se ubican como bloques de una caja antes de la busqueda
	if(fixed){
		for(const Block::PlacedBox& p : *fixed){
			const Block* blk=NULL;
			for(const Block* b : s0->valid_blocks)
				if(b->child[0]<0 && b->box_type==p.type && b->getL()==p.xmax-p.xmin &&
						b->getW()==p.ymax-p.ymin && b->getH()==p.zmax-p.zmin){ blk=b; break; }
			if(!blk){ cerr << "ERROR: fixed box not found" << endl; exit(1); }
			AABB a(p.xmin, p.ymin, p.zmin, p.xmax, p.ymax, p.zmax);
			s0->transition(clpAction(*blk, Space(a, u.L, u.W, u.H), p.xmin, p.ymin, p.zmin));
		}
	}

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
	//con P.time=0 solo se ejecuta el greedy (una completacion)
	if(P.time>0) de->run(*s_copy, P.time, begin_time);
	else gr->run(*s_copy, 0.0, begin_time);
	cout.rdbuf(buf);

	clpState* best = (P.time>0)? (de->get_best_state()? static_cast<clpState*>(de->get_best_state()->clone()) : NULL)
			: static_cast<clpState*>(s_copy->clone());
	if(best){
		//se desplaza la carga para centrar el centro de gravedad
		long dx, dy;
		best->cog_violation(dx, dy);
		if(center) best->translate(dx, dy);

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
 * carga el ULD u con las cajas left (espacios anclados en el origen). Con --corner_anchor,
 * si el centro de gravedad no queda dentro del rango permitido (o el ULD queda poco
 * utilizado y quedan cajas), se repite la busqueda anclando los espacios en el origen (la
 * carga queda agrupada y se puede centrar) y se elige la mejor de las dos cargas
 */
static Load load_cog(const ULD& u, const vector<int>& left, const Params& P,
		const vector<Block::PlacedBox>* fixed=NULL){
	Load res = load_bsg(u, left, P, fixed);
	//tambien si el ULD queda poco utilizado y quedan cajas (cargas pequenas: al agruparlas se
	//pueden centrar)
	double avail=0;
	for(size_t t=0; t<left.size(); t++) avail += (double) left[t]*clpState::types[t]->getVolume();
	bool small = res.utilization() < 0.5 && res.volume < avail;
	if(regroup && !origin_only && clpState::cog_penalty>0.0 && (!res.cog_ok() || small)){
		Space::origin_anchor=true;
		Load res2 = load_bsg(u, left, P, fixed);
		Space::origin_anchor=origin_only;
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
 * continua la carga del ULD de base (con sus cajas fijas en sus posiciones) agregando
 * cajas de extra
 */
static Load load_more(const Load& base, const vector<int>& extra, const Params& P){
	vector<int> avail = base.boxes;
	for(size_t t=0; t<avail.size(); t++) avail[t]+=extra[t];
	return load_cog(*base.uld, avail, P, &base.placed);
}

/** cajas de left que, entre los ULDs disponibles, solo caben en el tipo de u */
static vector<int> only_in(const vector<int>& left, const ULD& u){
	vector<int> res(left.size(), 0);
	int iu=uld_index(u);
	for(const BoxShape* b : clpState::types){
		if(left[b->type]==0 || !fits(*b, u)) continue;
		bool other=false;
		for(size_t i=0; i<ulds.size() && !other; i++)
			if((int)i!=iu && available[i]!=0 && fits(*b, ulds[i])) other=true;
		if(!other) res[b->type]=left[b->type];
	}
	return res;
}

/**
 * Saca de la carga ld cajas que no tienen nada encima (ni apoyado sobre ellas con relleno),
 * de arriba hacia abajo, hasta sacar la proporcion p del volumen cargado. El resto de la carga
 * sigue siendo factible (salvo el centro de gravedad)
 */
static Load ruin(const Load& ld, double p){
	const vector<Block::PlacedBox>& B=ld.placed;
	vector<bool> gone(B.size(), false);
	double removed=0;
	while(removed < p*ld.volume){
		int best=-1;
		for(size_t i=0; i<B.size(); i++){
			if(gone[i]) continue;
			bool covered=false;
			for(size_t j=0; j<B.size() && !covered; j++){
				if(gone[j] || j==i) continue;
				if(B[j].zmin >= B[i].zmax && B[j].zmin <= B[i].zmax + clpState::padding &&
						min(B[i].xmax, B[j].xmax) > max(B[i].xmin, B[j].xmin) &&
						min(B[i].ymax, B[j].ymax) > max(B[i].ymin, B[j].ymin)) covered=true;
			}
			if(!covered && (best<0 || B[i].zmax > B[best].zmax)) best=i;
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

/**
 * Ruina y reconstruccion para las cajas no cargadas freed (ULDs limitados): se saca la
 * proporcion p de cada ULD (ver ruin) y se reconstruyen (de mayor a menor volumen libre)
 * dejando fijas las cajas que quedaron y agregando las liberadas. Se acepta si queda menos
 * volumen sin cargar; freed queda con las cajas que no se cargaron.
 */
static bool ruin_recreate(vector<Load>& loads, vector<int>& freed, double p, const Params& P){
	vector<int> pool=freed;
	double pool_vol0=0;
	for(size_t t=0; t<pool.size(); t++) pool_vol0 += (double) pool[t]*clpState::types[t]->getVolume();

	vector<Load> new_loads=loads;
	vector<int> order(loads.size());
	for(size_t i=0; i<order.size(); i++) order[i]=i;
	sort(order.begin(), order.end(), [&](int a, int b){
		return loads[a].uld->volume - loads[a].volume > loads[b].uld->volume - loads[b].volume; });

	for(int i : order){
		Load base = ruin(loads[i], p);
		vector<int> avail=pool;
		for(size_t t=0; t<pool.size(); t++) avail[t] += loads[i].boxes[t]-base.boxes[t];
		Load ld = load_more(base, avail, P);
		if(!ld.feasible || (loads[i].cog_ok() && !ld.cog_ok())) continue;
		new_loads[i]=ld;
		for(size_t t=0; t<pool.size(); t++) pool[t] = avail[t] + base.boxes[t] - ld.boxes[t];
		if(count(pool.begin(), pool.end(), 0)==(int)pool.size()) break;
	}

	double pool_vol=0;
	for(size_t t=0; t<pool.size(); t++) pool_vol += (double) pool[t]*clpState::types[t]->getVolume();
	if(pool_vol >= pool_vol0) return false;
	loads=new_loads;
	freed=pool;
	return true;
}

//etapas de la insercion de cajas (ablacion)
static bool stage_scratch=true, stage_fixed=false, stage_priority=true, stage_ruin=true;

static double volume_of(const vector<int>& b){
	double v=0;
	for(size_t t=0; t<b.size(); t++) v += (double) b[t]*clpState::types[t]->getVolume();
	return v;
}

static bool none(const vector<int>& b){ return count(b.begin(), b.end(), 0)==(int)b.size(); }

/** la nueva carga ld del ULD reemplaza a old: carga mas volumen sin dejar el CoG fuera de rango si no lo estaba */
static bool improves(const Load& old, const Load& ld){
	return ld.feasible && !(old.cog_ok() && !ld.cog_ok()) && ld.volume > old.volume;
}

/** ULDs de mayor a menor volumen libre */
static vector<int> by_free_volume(const vector<Load>& loads){
	vector<int> order(loads.size());
	for(size_t i=0; i<order.size(); i++) order[i]=i;
	sort(order.begin(), order.end(), [&](int a, int b){
		return loads[a].uld->volume - loads[a].volume > loads[b].uld->volume - loads[b].volume; });
	return order;
}

/**
 * Inserta las cajas de pool en los ULDs cargados (sin agregar ULDs). En cada etapa se recorren
 * los ULDs de mayor a menor volumen libre y la nueva carga de un ULD se acepta si carga mas
 * volumen (ver improves); las cajas que el ULD deja de cargar vuelven a pool:
 *  1. desde cero: se carga el ULD con sus cajas y las de pool;
 *  2. con sus cajas fijas en sus posiciones y las de pool;
 *  3. con prioridad: se carga el ULD solo con las cajas de pool que caben en su tipo y se
 *     completa con sus cajas y el resto de pool;
 *  4. ruina y reconstruccion (ver ruin_recreate), mientras disminuya el volumen sin cargar.
 * pool queda con las cajas que no se insertaron.
 */
static void insert(vector<Load>& loads, vector<int>& pool, const Params& P){
	auto with_pool=[&](const Load& ld){
		vector<int> avail=ld.boxes;
		for(size_t t=0; t<avail.size(); t++) avail[t]+=pool[t];
		return avail;
	};
	auto take=[&](int i, const Load& ld, const vector<int>& avail){
		loads[i]=ld;
		for(size_t t=0; t<pool.size(); t++) pool[t]=avail[t]-ld.boxes[t];
	};

	if(stage_scratch)
		for(int i : by_free_volume(loads)){
			if(none(pool)) return;
			vector<int> avail=with_pool(loads[i]);
			Load ld=load(*loads[i].uld, avail, P);
			if(improves(loads[i], ld)) take(i, ld, avail);
		}

	if(stage_fixed)
		for(int i : by_free_volume(loads)){
			if(none(pool)) return;
			vector<int> avail=with_pool(loads[i]);
			Load ld=load_more(loads[i], pool, P);
			if(improves(loads[i], ld)) take(i, ld, avail);
		}

	if(stage_priority)
		for(int i : by_free_volume(loads)){
			if(none(pool)) return;
			const ULD& u=*loads[i].uld;
			vector<int> prio(pool.size(), 0);
			for(size_t t=0; t<pool.size(); t++) if(pool[t]>0 && fits(*clpState::types[t], u)) prio[t]=pool[t];
			if(none(prio)) continue;
			vector<int> avail=with_pool(loads[i]);
			Load ld2=load_bsg(u, prio, P, NULL, false);
			if(none(ld2.boxes)) continue;
			vector<int> rest=avail;
			for(size_t t=0; t<rest.size(); t++) rest[t]-=ld2.boxes[t];
			Load ld3=load_more(ld2, rest, P);
			const Load& ld=(ld3.feasible && ld3.volume>=ld2.volume)? ld3 : ld2;
			if(improves(loads[i], ld)) take(i, ld, avail);
		}

	if(stage_ruin)
		for(bool progress=true; progress && !none(pool); ){
			progress=false;
			for(double p : {0.3, 0.6, 0.9})
				if(!none(pool) && ruin_recreate(loads, pool, p, P)) progress=true;
		}
}

/**
 * Intenta eliminar un ULD (de menor a mayor utilizacion): se liberan sus cajas y se insertan
 * en los otros ULDs (ver insert). Retorna true si se elimino alguno.
 */
/**
 * Reduccion del volumen de los ULDs (--reduce): se saca un ULD (de mayor a menor volumen
 * perdido), sus cajas se insertan en los otros ULDs (ver insert) y, si sobran, se cargan en un
 * ULD nuevo del tipo de menor volumen posible, siempre menor que el del ULD sacado (sin dejar
 * el CoG fuera de rango si no lo estaba). Generaliza la eliminacion (no sobran cajas) y el
 * cambio a un ULD mas pequeno (no se inserta ninguna). Retorna true si se hizo un cambio.
 */
static int reduce_new=1;
static bool reduce_volume(vector<Load>& loads, const Params& P){
	vector<int> order(loads.size());
	for(size_t i=0; i<order.size(); i++) order[i]=i;
	auto waste=[&](int i){ return loads[i].uld->volume - loads[i].volume; };
	sort(order.begin(), order.end(), [&](int a, int b){ return waste(a) > waste(b); });
	for(int target : order){
		const Load& tg=loads[target];
		vector<int> pool=tg.boxes;
		vector<Load> others;
		for(size_t i=0; i<loads.size(); i++) if((int)i!=target) others.push_back(loads[i]);
		if(!others.empty()) insert(others, pool, P);
		if(none(pool)){ loads=others; return true; }
		//sobrante: 1 a reduce_new ULDs nuevos de menor volumen total que el sacado, de menor a
		//mayor volumen total (el mas grande se carga primero, con todo el sobrante)
		double pv=volume_of(pool), pw=0;
		for(size_t t=0; t<pool.size(); t++) pw += pool[t]*clpState::types[t]->weight;
		int kt=uld_index(*tg.uld);
		vector<int> av=available;
		if(av[kt]>=0) av[kt]++;
		vector< vector<int> > combos;
		for(size_t a=0; a<ulds.size(); a++){
			combos.push_back({(int)a});
			if(reduce_new>=2)
				for(size_t b=0; b<ulds.size(); b++)
					if(ulds[b].volume < ulds[a].volume || (ulds[b].volume == ulds[a].volume && b>=a)) combos.push_back({(int)a, (int)b});
		}
		auto cvol=[&](const vector<int>& C){ double v=0; for(int k : C) v+=ulds[k].volume; return v; };
		sort(combos.begin(), combos.end(), [&](const vector<int>& x, const vector<int>& y){ return cvol(x) < cvol(y); });
		for(const vector<int>& C : combos){
			double vc=cvol(C), wc=0;
			if(vc >= tg.uld->volume - 1e-6) break;
			if(vc < pv) continue;
			vector<int> a2=av; bool ok=true;
			for(int k : C){ wc+=ulds[k].max_weight; if(a2[k]==0) ok=false; else if(a2[k]>0) a2[k]--; }
			if(!ok || wc < pw) continue;
			for(size_t t=0; t<pool.size() && ok; t++){
				if(!pool[t]) continue;
				bool any=false;
				for(int k : C) any = any || fits(*clpState::types[t], ulds[k]);
				ok=any;
			}
			if(!ok) continue;
			vector<Load> nl; vector<int> rest=pool;
			for(int k : C){
				if(none(rest)) break;
				Load ld=load(ulds[k], rest, P);
				if(!ld.feasible || none(ld.boxes) || (tg.cog_ok() && !ld.cog_ok())){ ok=false; break; }
				for(size_t t=0; t<rest.size(); t++) rest[t]-=ld.boxes[t];
				nl.push_back(ld);
			}
			if(!ok || !none(rest)) continue;
			//ULDs de C no usados (el primero ya cargo todo)
			for(size_t i=nl.size(); i<C.size(); i++) if(a2[C[i]]>=0) a2[C[i]]++;
			available=a2;
			for(const Load& ld : nl) others.push_back(ld);
			loads=others;
			return true;
		}
	}
	return false;
}

static bool eliminate_uld(vector<Load>& loads, const Params& P){
	if(loads.size()<2) return false;
	vector<int> order(loads.size());
	for(size_t i=0; i<order.size(); i++) order[i]=i;
	sort(order.begin(), order.end(), [&](int a, int b){ return loads[a].utilization() < loads[b].utilization(); });
	for(int target : order){
		vector<int> pool=loads[target].boxes;
		vector<Load> others;
		for(size_t i=0; i<loads.size(); i++) if((int)i!=target) others.push_back(loads[i]);
		insert(others, pool, P);
		if(none(pool)){ loads=others; return true; }
	}
	return false;
}

/**
 * Reasignacion para reducir el volumen total de los ULDs (el volumen perdido): se toman 1 o 2
 * ULDs (de mayor a menor volumen perdido) y se intenta cargar sus cajas en 1 a max_new ULDs
 * (tipos disponibles) de menor volumen total, probando las combinaciones de menor a mayor
 * volumen (de mayor a menor ULD, cada uno con las cajas que no cargaron los anteriores). Se acepta el primer cambio que carga todas las cajas sin aumentar la cantidad
 * de ULDs con el CoG fuera de rango. Retorna true si se hizo un cambio.
 */
static int max_new=3;
//seleccion del siguiente ULD entre los candidatos: theta>=0 maximiza (volumen cargado) -
//theta*(volumen del ULD) (theta<0: regla de Hessler et al.); con rollout, se completa la
//solucion con una construccion rapida y se elige el de menor volumen total
static double theta=-1.0;
static bool rollout=false;
static double rollout_time=0.01;
//conjuntos de cajas candidatos (--subsets): proporciones del volumen del ULD
static vector<double> subset_rhos;

/**
 * Completa la carga de las cajas left con la construccion secuencial (regla de Hessler et al.,
 * sin fases de mejora) y P.time=rollout_time; los ULDs disponibles se restauran al final.
 * Retorna el volumen sin cargar, el volumen de los ULDs y la cantidad de ULDs.
 */
struct Estimate { double unloaded, volume; int n; };
static Estimate complete(vector<int> left, const Params& P0){
	Params P=P0; P.time=rollout_time;
	vector<int> av=available;
	Estimate e{0, 0, 0};
	while(!none(left)){
		const ULD* u=select_uld(left);
		if(!u) break;
		Load ld=load(*u, left, P);
		if(none(ld.boxes)) break;
		for(size_t t=0; t<left.size(); t++) left[t]-=ld.boxes[t];
		e.volume+=u->volume; e.n++;
		int k=uld_index(*u); if(available[k]>0) available[k]--;
	}
	e.unloaded=volume_of(left);
	available=av;
	return e;
}
//reasignacion: utilizacion maxima exigible a una combinacion y tiempo de BSG (proporcion de t)
static double reassign_umax=1.0, reassign_tfrac=1.0;
static bool reassign(vector<Load>& loads, const Params& P0){
	Params P=P0; P.time=P0.time*reassign_tfrac;
	int n=loads.size(), nt=ulds.size();
	//conjuntos de 1 o 2 ULDs, de mayor a menor volumen perdido
	vector< vector<int> > sets;
	for(int i=0; i<n; i++) sets.push_back({i});
	for(int i=0; i<n; i++) for(int j=i+1; j<n; j++) sets.push_back({i, j});
	auto waste=[&](const vector<int>& S){
		double w=0;
		for(int i : S) w += loads[i].uld->volume - loads[i].volume;
		return w;
	};
	stable_sort(sets.begin(), sets.end(), [&](const vector<int>& a, const vector<int>& b){ return waste(a) > waste(b); });
	//combinaciones de 1 a max_new tipos (indices en ulds), de mayor a menor volumen
	vector<int> idx(nt);
	for(int k=0; k<nt; k++) idx[k]=k;
	sort(idx.begin(), idx.end(), [&](int a, int b){ return ulds[a].volume > ulds[b].volume; });
	vector< vector<int> > combos;
	vector<int> cur;
	function<void(int)> gen=[&](int from){
		if(!cur.empty()) combos.push_back(cur);
		if((int)cur.size()==max_new) return;
		for(int p=from; p<nt; p++){ cur.push_back(idx[p]); gen(p); cur.pop_back(); }
	};
	gen(0);
	auto cvol=[&](const vector<int>& C){ double v=0; for(int k : C) v+=ulds[k].volume; return v; };
	stable_sort(combos.begin(), combos.end(), [&](const vector<int>& a, const vector<int>& b){ return cvol(a) < cvol(b); });

	for(const vector<int>& S : sets){
		vector<int> boxes(clpState::types.size(), 0);
		double vS=0, bvol=0, bw=0; int cogS=0;
		for(int i : S){
			for(size_t t=0; t<boxes.size(); t++) boxes[t]+=loads[i].boxes[t];
			vS+=loads[i].uld->volume; bvol+=loads[i].volume; bw+=loads[i].weight; cogS+=!loads[i].cog_ok();
		}
		for(const vector<int>& C : combos){
			double vC=cvol(C);
			if(vC >= vS - 1e-6) break;
			if(vC*reassign_umax < bvol) continue;
			//disponibilidad (se liberan los ULDs de S)
			vector<int> av=available;
			for(int i : S){ int k=uld_index(*loads[i].uld); if(av[k]>=0) av[k]++; }
			bool ok=true; double cw=0;
			for(int k : C){ if(av[k]==0) ok=false; else if(av[k]>0) av[k]--; cw+=ulds[k].max_weight; }
			if(!ok || cw < bw) continue;
			//cada caja cabe en algun tipo de C
			for(size_t t=0; t<boxes.size() && ok; t++){
				if(!boxes[t]) continue;
				bool any=false;
				for(int k : C) any = any || fits(*clpState::types[t], ulds[k]);
				ok=any;
			}
			if(!ok) continue;
			vector<Load> nl;
			vector<int> rest=boxes;
			for(int k : C){
				if(none(rest)) break;
				Load ld=load(ulds[k], rest, P);
				if(!ld.feasible || none(ld.boxes)){ ok=false; break; }
				for(size_t t=0; t<rest.size(); t++) rest[t]-=ld.boxes[t];
				nl.push_back(ld);
			}
			if(!ok || !none(rest)) continue;
			int cogC=0; for(const Load& ld : nl) cogC+=!ld.cog_ok();
			if(cogC > cogS) continue;
			//se reemplazan los ULDs de S por los de C
			vector<Load> res;
			for(int i=0; i<n; i++) if(find(S.begin(), S.end(), i)==S.end()) res.push_back(loads[i]);
			for(const Load& ld : nl) res.push_back(ld);
			available=av;
			for(size_t k=0; k<C.size(); k++) if(k>=nl.size()){ int kk=C[k]; if(available[kk]>=0) available[kk]++; }
			loads=res;
			return true;
		}
	}
	return false;
}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-ULD *********.", "BSG Solver for air cargo loading (ULDs).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<string> _uld(parser, "string", "ULD file (default: uld.csv in the parent directory of the instance)", {"uld"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _max_bl(parser, "int", "Maximum number of blocks. Default: 500", {"max_bl"});
	args::ValueFlag<double> _maxtime(parser, "double", "Timelimit of the search in each ULD (seconds, without the block generation). Default: 1", {'t', "timelimit"});
	args::ValueFlag<double> _cog_greedy(parser, "double", "Penalty of the deviation of the center of gravity in the evaluation of the blocks. Default: 0.5987", {"cog_greedy"});
	args::ValueFlag<double> _cog_penalty(parser, "double", "Penalty of the deviation of the center of gravity (relative to the allowed one) in the evaluation of the solutions. Default: 12.6619", {"cog_penalty"});
	args::ValueFlag<string> _support(parser, "string", "Support of the boxes: corners (the four bottom corners rest on non-fragile boxes or on the floor, as in Paquay et al. 2018), full (the whole base) or none. Default: corners", {"support"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter. Default: 1.2335", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter. Default: 0.2389", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter. Default: 0.2243", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter. Default: 0.0574", {'p'});
	args::Flag _fix_knapsack(parser, "fix_knapsack", "Volume loss: consider the lengths of all the allowed orientations of the boxes", {"fix_knapsack"});
	args::ValueFlag<long> _edge(parser, "int", "Width of the edge of the ULD floor on which loading is not allowed below the height of the vertical edge offset (Hessler et al. 2024). Default: 0", {"edge"});
	args::ValueFlag<long> _edge_offset(parser, "int", "Vertical edge offset (with an edge, a substructure of this height can also be used). Default: 0", {"edge_offset"});
	args::ValueFlag<double> _min_overlap(parser, "double", "Support: a box is also supported if part of its base rests on other boxes and the proportion min_overlap of its base rests on boxes or on padding (Hessler et al. 2024). Default: 0 (only the four corners)", {"min_overlap"});
	args::ValueFlag<long> _padding(parser, "int", "Maximum height of the padding material. Default: 0", {"padding"});
	args::ValueFlag<double> _cog_rel(parser, "double", "Maximum deviation of the center of gravity in x and y relative to the half dimensions of the ULD (Hessler et al. 2024; default: the deviations of the ULD file)", {"cog_rel"});
	args::ValueFlag<int> _uld_count(parser, "int", "Number of available ULDs of each type. Default: unlimited", {"uld_count"});
	args::Flag _no_translate(parser, "no_translate", "Ablation: do not translate the load to center its center of gravity", {"no_translate"});
	args::Flag _corner_anchor(parser, "corner_anchor", "Ablation: anchor each space at its closest corner of the ULD (instead of the origin) and, if the center of gravity is out of range, load the ULD again with the spaces anchored at the origin", {"corner_anchor"});
	args::Flag _no_regroup(parser, "no_regroup", "Ablation (with --corner_anchor): do not load again a ULD anchoring the spaces at the origin", {"no_regroup"});
	args::Flag _no_substructure(parser, "no_substructure", "Ablation: do not use a substructure", {"no_substructure"});
	args::Flag _no_scratch(parser, "no_scratch", "Ablation: insertion of boxes without the reload from scratch", {"no_scratch"});
	args::Flag _fixed(parser, "fixed", "Ablation: insertion of boxes with a reload with fixed boxes", {"fixed"});

	args::Flag _no_priority(parser, "no_priority", "Ablation: insertion of boxes without the reload with priority", {"no_priority"});
	args::Flag _no_ruin(parser, "no_ruin", "Ablation: insertion of boxes without ruin and recreate", {"no_ruin"});
	args::Flag _reassign(parser, "reassign", "Reduce the total volume of the ULDs by loading the boxes of 1 or 2 ULDs into 1 to --max_new ULDs of smaller total volume", {"reassign"});
	args::ValueFlag<double> _theta(parser, "double", "Select the next ULD type among the candidates (rule of Hessler et al.) by loading each one and maximizing loaded volume - theta * ULD volume (0: maximum loaded volume, 1: minimum lost volume). Default: rule of Hessler et al.", {"theta"});
	args::Flag _rollout(parser, "rollout", "Select the next ULD type among the candidates by completing the solution with a fast construction (see --rollout_time) and minimizing the total volume of the ULDs", {"rollout"});
	args::ValueFlag<string> _subsets(parser, "string", "Candidate sets of boxes for each ULD, evaluated with the rollout: the largest boxes (or the boxes that fit in fewer ULD types, then the largest) up to rho times the volume of the ULD, for each rho in the comma-separated list", {"subsets"});
	args::ValueFlag<double> _rollout_time(parser, "double", "Time limit of each run of BSG in the rollout (0: only the greedy). Default: 0.01", {"rollout_time"});
	args::ValueFlag<double> _reassign_umax(parser, "double", "Reassignment: skip combinations that would need a utilization above this value. Default: 1", {"reassign_umax"});
	args::ValueFlag<double> _reassign_tfrac(parser, "double", "Reassignment: time limit of BSG as a proportion of -t. Default: 1", {"reassign_tfrac"});
	args::ValueFlag<int> _max_new(parser, "int", "Reassignment: maximum number of new ULDs. Default: 3", {"max_new"});
	args::Flag _reduce(parser, "reduce", "Reduce the volume of the ULDs: remove a ULD, insert its boxes into the others and load the remaining ones into a smaller ULD (generalizes the elimination of ULDs)", {"reduce"});
	args::ValueFlag<int> _reduce_new(parser, "int", "Reduction of the volume: maximum number of new ULDs for the remaining boxes (1 or 2). Default: 1", {"reduce_new"});
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
	P.alpha=1.2335; P.beta=0.2389; P.gamma=0.2243; P.delta=1.0; P.p=0.0574;
	if(_alpha) P.alpha=_alpha.Get();
	if(_beta) P.beta=_beta.Get();
	if(_gamma) P.gamma=_gamma.Get();
	if(_delta) P.delta=_delta.Get();
	if(_p) P.p=_p.Get();
	P.min_fr=(_min_fr)? _min_fr.Get():0.98;
	P.time=(_maxtime)? _maxtime.Get():1.0;
	P.max_bl=(_max_bl)? _max_bl.Get():500;
	P.fix_knapsack=_fix_knapsack;
	string support=(_support)? _support.Get() : "corners";
	if(support!="corners" && support!="full" && support!="none"){
		cerr << "Invalid support: " << support << endl;
		return 1;
	}
	clpState::support=(support!="none");
	clpState::corner_support=(support=="corners");
	clpState::cog_penalty=(_cog_penalty)? _cog_penalty.Get() : 12.6619;
	clpState::cog_greedy=(_cog_greedy)? _cog_greedy.Get() : 0.5987;

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
	use_substructure = !_no_substructure && clpState::corner_support && ulds[0].edge>0 && ulds[0].delta>0;
	regroup = !_no_regroup;
	if(_max_new) max_new=_max_new.Get();
	if(_reduce_new) reduce_new=_reduce_new.Get();
	if(_reassign_umax) reassign_umax=_reassign_umax.Get();
	if(_reassign_tfrac) reassign_tfrac=_reassign_tfrac.Get();
	if(_theta) theta=_theta.Get();
	rollout=_rollout;
	if(_rollout_time) rollout_time=_rollout_time.Get();
	if(_subsets){ stringstream ss(_subsets.Get()); string tok; while(getline(ss, tok, ',')) subset_rhos.push_back(atof(tok.c_str())); }
	origin_only = !_corner_anchor;
	Space::origin_anchor = origin_only;
	clpState::translate_cog = !_no_translate;
	//con borde, el espacio sobre el borde no tiene apoyo hasta que se cargan cajas dentro de el
	clpState::dormant_spaces = (ulds[0].edge>0);
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
	bool limited = (_uld_count && _uld_count.Get()>0);

	while(remaining()>0){
		vector<const ULD*> cands;
		const ULD* u = select_uld(left, &cands);
		//no quedan ULDs disponibles en que quepan las cajas restantes
		if(!u) break;
		Load ld = load(*u, left, P);
		//eleccion entre los tipos candidatos (--theta, --rollout): se carga cada uno; no se
		//elige uno que deje el CoG fuera de rango si otro no
		if((theta>=0 || rollout) && cands.size()>1){
			auto est=[&](const Load& l){
				vector<int> rest=left;
				for(size_t t=0; t<rest.size(); t++) rest[t]-=l.boxes[t];
				int k=uld_index(*l.uld);
				if(available[k]>0) available[k]--;
				Estimate e=complete(rest, P);
				if(available[k]>=0 && available[k]!=-1) available[k]++;
				e.volume+=l.uld->volume; e.n++;
				return e;
			};
			auto better_est=[](const Estimate& a, const Estimate& b){
				if(fabs(a.unloaded-b.unloaded)>1e6) return a.unloaded<b.unloaded;
				if(fabs(a.volume-b.volume)>1e6) return a.volume<b.volume;
				return a.n<b.n;
			};
			Estimate e0; if(rollout) e0=est(ld);
			auto score=[&](const Load& l){ return l.volume - theta*l.uld->volume; };
			for(const ULD* c : cands){
				if(c->type==u->type) continue;
				Load ld2 = load(*c, left, P);
				if(!ld2.feasible || none(ld2.boxes) || (ld.cog_ok() && !ld2.cog_ok())) continue;
				bool take=(!ld.cog_ok() && ld2.cog_ok());
				if(!take && rollout){
					Estimate e2=est(ld2);
					if(better_est(e2, e0)){ take=true; e0=e2; }
				}else if(!take) take = score(ld2) > score(ld);
				else if(rollout) e0=est(ld2);
				if(take){ ld=ld2; u=c; }
			}
		}
		//conjuntos de cajas candidatos para este ULD (--subsets): las mas grandes (o las que caben
		//en menos tipos y luego las mas grandes) hasta rho veces el volumen del ULD, para cada
		//rho; se elige con rollout la carga de menor volumen total estimado
		if(!subset_rhos.empty()){
			auto est=[&](const Load& l){
				vector<int> rest=left;
				for(size_t t=0; t<rest.size(); t++) rest[t]-=l.boxes[t];
				int k=uld_index(*l.uld);
				if(available[k]>0) available[k]--;
				Estimate e=complete(rest, P);
				if(available[k]>=0) available[k]++;
				e.volume+=l.uld->volume; e.n++;
				return e;
			};
			auto better_est=[](const Estimate& a, const Estimate& b){
				if(fabs(a.unloaded-b.unloaded)>1e6) return a.unloaded<b.unloaded;
				if(fabs(a.volume-b.volume)>1e6) return a.volume<b.volume;
				return a.n<b.n;
			};
			Estimate e0=est(ld);
			vector<int> ord;
			for(size_t t=0; t<left.size(); t++) if(left[t]>0 && fits(*clpState::types[t], *u)) ord.push_back(t);
			auto nfit=[&](int t){ int c=0; for(size_t k=0; k<ulds.size(); k++) c += available[k]!=0 && fits(*clpState::types[t], ulds[k]); return c; };
			for(int hard=0; hard<2; hard++){
				vector<int> o=ord;
				stable_sort(o.begin(), o.end(), [&](int a, int b){
					if(hard && nfit(a)!=nfit(b)) return nfit(a)<nfit(b);
					return clpState::types[a]->getVolume() > clpState::types[b]->getVolume(); });
				for(double rho : subset_rhos){
					vector<int> sub(left.size(), 0);
					double v=0;
					for(int t : o)
						for(int c=0; c<left[t] && v < rho*u->volume; c++){ sub[t]++; v+=clpState::types[t]->getVolume(); }
					if(sub==left) continue;
					Load ls=load(*u, sub, P);
					if(!ls.feasible || none(ls.boxes) || (ld.cog_ok() && !ls.cog_ok())) continue;
					Estimate e=est(ls);
					if(better_est(e, e0)){ e0=e; ld=ls; }
				}
			}
		}
		//con ULDs limitados: si quedan fuera cajas que solo caben en este ULD (entre los
		//disponibles), se carga el ULD priorizando esas cajas
		if(limited){
			vector<int> must=only_in(left, *u);
			double must_vol=0, loaded_must=0;
			for(size_t t=0; t<must.size(); t++){
				must_vol += (double) must[t]*clpState::types[t]->getVolume();
				loaded_must += (double) min(must[t], ld.boxes[t])*clpState::types[t]->getVolume();
			}
			if(loaded_must < must_vol){
				//(sin centrar la carga, que se completa a continuacion)
				Load ld2 = load_bsg(*u, must, P, NULL, false);
				double loaded2=0;
				for(size_t t=0; t<must.size(); t++) loaded2 += (double) ld2.boxes[t]*clpState::types[t]->getVolume();
				if(loaded2 > loaded_must){
					//se completa el ULD con las otras cajas
					vector<int> rest=left;
					for(size_t t=0; t<rest.size(); t++) rest[t]-=ld2.boxes[t];
					Load ld3 = load_more(ld2, rest, P);
					ld = (ld3.feasible && ld3.volume >= ld2.volume)? ld3 : ld2;
				}
			}
		}
		int n=0; for(int x : ld.boxes) n+=x;
		if(n==0){
			cerr << "ERROR: no box was loaded in ULD " << u->type << endl;
			return 1;
		}
		for(size_t t=0; t<left.size(); t++) left[t]-=ld.boxes[t];
		loads.push_back(ld);
		if(available[uld_index(*u)]>0) available[uld_index(*u)]--;
	}

	stage_scratch=!_no_scratch; stage_fixed=_fixed; stage_priority=!_no_priority; stage_ruin=!_no_ruin;

	//cajas no cargadas (ULDs limitados): se insertan en los ULDs cargados
	if(remaining()>0) insert(loads, left, P);

	//si se cargaron todas las cajas, se eliminan ULDs insertando sus cajas en los otros (con
	//--reduce: reduccion del volumen, que generaliza la eliminacion)
	if(!_no_eliminate && remaining()==0){
		if(_reduce) while(reduce_volume(loads, P));
		else while(eliminate_uld(loads, P));
	}

	//reasignacion para reducir el volumen de los ULDs (despues, se intenta eliminar de nuevo)
	if(_reassign)
		while(reassign(loads, P))
			if(!_no_eliminate && remaining()==0) while(eliminate_uld(loads, P));

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
