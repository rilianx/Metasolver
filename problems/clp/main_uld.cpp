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

	double utilization() const { return volume/uld->volume; }
	bool cog_ok() const {
		return fabs(cog[0]-uld->L/2.0) <= uld->dev[0] && fabs(cog[1]-uld->W/2.0) <= uld->dev[1];
	}
};

/** lee las cajas (formato de Paquay et al.) agrupando las identicas en tipos */
static vector<int> read_items(const string& file){
	ifstream in(file.c_str());
	if(!in){
		cerr << "No se pudo abrir el archivo " << file << endl;
		exit(1);
	}
	//cada linea: l;w;h;peso;l vertical;w vertical;h vertical;fragil;muy densa
	map<vector<double>, int> count;
	vector< vector<double> > order;
	string line;
	while(getline(in, line)){
		vector<double> f;
		stringstream ss(line);
		string tok;
		while(getline(ss, tok, ';')) if(!tok.empty()) f.push_back(atof(tok.c_str()));
		if(f.size()<9) continue;
		f.resize(8);
		if(count[f]++ == 0) order.push_back(f);
	}

	vector<int> n;
	for(auto& f : order){
		int t=clpState::types.size();
		clpState::types.push_back(new BoxShape(t, t, (long) f[0], (long) f[1], (long) f[2],
				f[4]!=0, f[5]!=0, f[6]!=0, f[3], f[7]!=0));
		n.push_back(count[f]);
	}
	return n;
}

/** la caja t cabe sola en el ULD u (sobre el piso si hay soporte) */
static bool fits(const BoxShape& box, const ULD& u){
	if(box.weight > u.max_weight) return false;
	for(int o=0; o<6; o++){
		if(!box.is_valid(o)) continue;
		long l=box.getL(o), w=box.getW(o), h=box.getH(o);
		if(l>u.L || w>u.W || h>u.H) continue;
		long x=0, z=0;
		if(u.adjust(x, z, l, h, 0, u.L-l, 0, u.H-h, !clpState::support)) return true;
	}
	return false;
}

/**
 * Siguiente ULD (Hessler et al. 2024): entre los ULDs en que caben las cajas que caben
 * en menos tipos de ULD, el de mayor volumen de cajas que caben en el (desempate: mayor volumen)
 */
static const ULD* select_uld(const vector<ULD>& ulds, const vector<int>& left){
	int m=ulds.size()+1;
	vector< vector<bool> > F(clpState::types.size(), vector<bool>(ulds.size()));
	for(const BoxShape* b : clpState::types){
		if(left[b->type]==0) continue;
		int nf=0;
		for(size_t u=0; u<ulds.size(); u++) nf += (F[b->type][u] = fits(*b, ulds[u]));
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

struct Params {
	double alpha, beta, gamma, p, delta, min_fr, time;
	bool fix_knapsack;
};

/** carga el ULD u con las cajas left usando BSG (P.time segundos de busqueda) */
static Load load(const ULD& u, const vector<int>& left, const Params& P){
	Load res;
	res.uld=&u;
	res.boxes.assign(left.size(), 0);
	res.volume=res.weight=0.0;
	res.cog[0]=res.cog[1]=res.cog[2]=0.0;
	res.feasible=true;

	clpState* s0 = new_state(u, left, P.min_fr, 10000);
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

	const clpState* best = static_cast<const clpState*>(de->get_best_state());
	if(best){
		for(size_t t=0; t<left.size(); t++) res.boxes[t] = left[t] - best->left[t];
		res.volume = best->cont.getOccupiedVolume();
		res.weight = best->get_weight();
		best->get_cog(res.cog[0], res.cog[1], res.cog[2]);
		res.feasible = best->is_feasible();
	}

	delete de; delete bsg; delete gr; delete vcs; delete s0;
	return res;
}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-ULD *********.", "BSG Solver for air cargo loading (ULDs).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<string> _uld(parser, "string", "ULD file (default: uld.csv in the parent directory of the instance)", {"uld"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<double> _maxtime(parser, "double", "Timelimit of the search in each ULD (seconds, without the block generation). Default: 1", {'t', "timelimit"});
	args::Flag _no_support(parser, "no_support", "Do not require full support of the blocks", {"no_support"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter. Default: 2.4407", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter. Default: 2.2652", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter. Default: 0.3896", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter. Default: 0.0164", {'p'});
	args::Flag _fix_knapsack(parser, "fix_knapsack", "Volume loss: consider the lengths of all the allowed orientations of the boxes", {"fix_knapsack"});
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
	P.fix_knapsack=_fix_knapsack;
	clpState::support=!_no_support;

	clock_t begin_time=clock();

	vector<ULD> ulds=ULD::read(uld_file);
	vector<int> left=read_items(file);

	for(const BoxShape* b : clpState::types){
		bool any=false;
		for(const ULD& u : ulds) any = any || fits(*b, u);
		if(!any){
			cerr << "ERROR: box " << b->type << " does not fit in any ULD" << endl;
			return 1;
		}
	}

	vector<Load> loads;
	auto remaining=[&](){ int n=0; for(int x : left) n+=x; return n; };

	while(remaining()>0){
		const ULD* u = select_uld(ulds, left);
		Load ld = load(*u, left, P);
		int n=0; for(int x : ld.boxes) n+=x;
		if(n==0){
			cerr << "ERROR: no box was loaded in ULD " << u->type << endl;
			return 1;
		}
		for(size_t t=0; t<left.size(); t++) left[t]-=ld.boxes[t];
		loads.push_back(ld);
	}

	//se intenta recargar las cajas del ultimo ULD en el ULD mas pequeno posible
	vector<const ULD*> smaller;
	for(const ULD& u : ulds) if(u.volume < loads.back().uld->volume) smaller.push_back(&u);
	sort(smaller.begin(), smaller.end(), [](const ULD* a, const ULD* b){ return a->volume < b->volume; });
	for(const ULD* u : smaller){
		const vector<int>& last=loads.back().boxes;
		bool all_fit=true;
		for(size_t t=0; t<last.size(); t++)
			if(last[t]>0 && !fits(*clpState::types[t], *u)) all_fit=false;
		if(!all_fit) continue;
		Load ld = load(*u, last, P);
		if(ld.boxes==last){ loads.back()=ld; break; }
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
			cout << "ULD " << i+1 << " type " << ld.uld->type << ": boxes " << n
				<< " utilization " << util[i] << "% weight " << ld.weight << "/" << ld.uld->max_weight
				<< " cog (" << ld.cog[0]-ld.uld->L/2.0 << "," << ld.cog[1]-ld.uld->W/2.0 << ")"
				<< (ld.cog_ok()? "":" violated") << endl;
		}
	}
	cout << "n_ulds " << loads.size() << " median_util " << median
		<< " total_util " << 100*total_vol/total_uld_vol << " cost " << cost
		<< " cog_violations " << cog_viol << " time " << time << endl;

	if(!feasible){
		cerr << "ERROR: infeasible solution" << endl;
		return 1;
	}
	return 0;
}
