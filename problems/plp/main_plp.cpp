/*
 * main_plp.cpp
 *
 * Carga de poliedros (3D irregular strip packing, sin rotaciones)
 *
 * 1. construccion: greedy (o BSG con --init bsg)
 * 2. oscilacion estrategica: se baja la altura y se separan las piezas con movimientos de minimo solapamiento
 *    por los ejes (busqueda local guiada); si se logra, se compacta con un LP; si no, se sube un poco la altura
 *    y, al volver a la altura de la mejor solucion, kick (se reinserta una pieza)
 *
 * ejemplo: BSG_PLP "problems/plp/benchs/Stoyan et al. 2005/Example1.json" -t 30 -o sol.json
 */

#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include "args.hxx"
#include "plpState.h"
#include "LPBackend.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "BSG.h"

using namespace std;
using namespace plp;

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-PLP *********.", "Solver for 3D irregular strip packing (polyhedra, ESICUP format).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit (seconds). Default: 30", {'t', "timelimit"});
	args::ValueFlag<unsigned> _seed(parser, "int", "Seed (0: no perturbation of the starting points). Default: 0", {"seed"});
	args::ValueFlag<string> _out(parser, "file", "Output solution (ESICUP JSON)", {'o', "output"});
	args::ValueFlag<string> _root(parser, "dir", "Root folder of the piece paths", {"root"});
	args::ValueFlag<string> _dec(parser, "dir", "Folder (under the root) with the convex decompositions of the pieces. Default: decompositions", {"dec"});
	args::Group construction(parser, "Construction:");
	args::ValueFlag<string> _init(construction, "greedy|bsg", "Initial solution: greedy or BSG (beam search guided by the greedy). Default: greedy", {"init"});
	args::ValueFlag<double> _bsg_time(construction, "double", "--init bsg: proportion of the time for the BSG. Default: 0.3", {"bsg_time"});
	args::ValueFlag<double> _alpha(construction, "double", "Weight of the volume of the piece. Default: 1", {"alpha"});
	args::ValueFlag<double> _beta(construction, "double", "Weight of the height of the piece. Default: 5.53", {"beta"});
	args::ValueFlag<double> _gamma(construction, "double", "Weight of the increase of the height. Default: 0.77", {"gamma"});
	args::ValueFlag<int> _grid(construction, "int", "Starting points per axis (x,y). Default: 4", {"grid"});
	args::ValueFlag<double> _slide(construction, "double", "Weight of x,y (towards the walls) in the LP descent. Default: 0.065", {"slide"});
	args::ValueFlag<int> _beams(construction, "int", "BSG: initial number of beams. Default: 4", {"beams"});
	args::ValueFlag<int> _blocks(construction, "int", "Maximum number of pseudo-blocks (pairs of pieces in a compact relative position). Default: 0", {"blocks"});
	args::ValueFlag<int> _block_pair(construction, "int", "Pseudo-blocks per pair of item types. Default: 2", {"block_pair"});
	args::ValueFlag<double> _block_waste(construction, "double", "Maximum added waste of a pseudo-block (convex hull of the block - convex hulls of the pieces) / volume of the pieces. Default: 0.07", {"block_waste"});
	args::ValueFlag<string> _start(construction, "file", "Start from this solution (ESICUP JSON) instead of the construction; e.g., a solution found with a coarser decomposition (--dec)", {"start"});
	args::Flag _greedy(construction, "greedy", "Only the construction (and the compaction)", {"greedy"});
	args::Group improvement(parser, "Improvement:");
	args::ValueFlag<double> _sep_step(improvement, "double", "Height decrease after a successful separation (proportion); on failure the height grows a quarter of it. Default: 0.01", {"sep_step"});
	args::ValueFlag<int> _gls_iter(improvement, "int", "Maximum passes of the minimum-overlap local search. Default: 500", {"gls_iter"});
	args::ValueFlag<double> _axis_window(improvement, "double", "Moves along the axes: a piece moves at most this multiple of its extent along the axis (0: the whole container). Default: 0", {"axis_window"});
	args::ValueFlag<string> _gls(improvement, "add|mult", "Guided local search weights: +1 for the overlapping pairs (add) or multiplicative by severity with decay (mult). Default: mult", {"gls"});
	args::Flag _sep_lp(improvement, "sep_lp", "If the moves along the axes fail, try the LP separation (requires SoPlex)", {"sep_lp"});
	args::ValueFlag<int> _sep_iter(improvement, "int", "Maximum iterations of the LP separation. Default: 50", {"sep_iter"});
	args::ValueFlag<string> _kick(improvement, "reinsert|swap|random|mix|none|top|topbest", "Kick: reinsert a piece in the free space, swap two pieces of different type and similar volume, move a piece to a random position (the separation resolves the overlap), or reinsert/random (1/2 each); none: only restart from the best solution; top: move one of the pieces that exceed the target height to a random position; topbest: the same, at the position of least penetration among --kick_samples random positions. Default: mix", {"kick"});
	args::ValueFlag<int> _kick_samples(improvement, "int", "--kick topbest: number of random positions. Default: 20", {"kick_samples"});
	args::Flag _no_compact(improvement, "no_compact", "Do not use the global LP compaction (requires SoPlex)", {"no_compact"});
	args::ValueFlag<double> _margin(improvement, "double", "Final compaction with this separation between neighboring parts, so that the solution can be certified in exact arithmetic (tools/certify_solution.py; requires SoPlex; 0: no). Default: 1e-7", {"margin"});
	args::Flag _repair(improvement, "repair", "After --kicks kicks without improvement, try to repair the best solution with the BSG in a lower container", {"repair"});
	args::ValueFlag<int> _kicks(improvement, "int", "--repair: consecutive kicks without improvement before the repair. Default: 30", {"kicks"});
	args::ValueFlag<double> _step(improvement, "double", "--repair: height decrease (proportion). Default: 0.03", {"step"});
	args::ValueFlag<double> _band(improvement, "double", "--repair: the pieces of the best solution below (1-band)*H are kept. Default: 0.2", {"band"});
	args::ValueFlag<double> _slice(improvement, "double", "--repair: maximum time (proportion of the timelimit). Default: 0.1", {"slice"});
	args::Positional<std::string> _file(parser, "instance", "Instance (ESICUP JSON)");

	cout.precision(8);
	try{
		parser.ParseCLI(argc, argv);
	}catch (args::Help&){
		std::cout << parser;
		return 0;
	}catch (args::ParseError& e){
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	if(!_file){
		std::cerr << parser;
		return 1;
	}

	int maxtime = (_maxtime)? _maxtime.Get() : 30;
	//ajustados con Optuna (problems/plp/results/ajuste.md)
	double alpha = (_alpha)? _alpha.Get() : 1.0;
	double beta = (_beta)? _beta.Get() : 5.53;
	double gamma = (_gamma)? _gamma.Get() : 0.77;
	int grid = (_grid)? _grid.Get() : 4;
	double slide = (_slide)? _slide.Get() : 0.065;
	int beams = (_beams)? _beams.Get() : 4;
	string init = (_init)? _init.Get() : "greedy";
	if(init!="greedy" && init!="bsg") { cerr << "--init debe ser greedy o bsg" << endl; return 1; }
	string kick = (_kick)? _kick.Get() : "mix";
	if(kick!="reinsert" && kick!="swap" && kick!="random" && kick!="mix" && kick!="none" && kick!="top" && kick!="topbest") {
		cerr << "--kick debe ser reinsert, swap, random, mix, none, top o topbest" << endl; return 1;
	}
	int kick_samples = (_kick_samples)? _kick_samples.Get() : 20;
	string gls = (_gls)? _gls.Get() : "mult";
	if(gls!="add" && gls!="mult") { cerr << "--gls debe ser add o mult" << endl; return 1; }
	double sep_step = (_sep_step)? _sep_step.Get() : 0.01;
	int gls_iter = (_gls_iter)? _gls_iter.Get() : 500;
	int sep_iter = (_sep_iter)? _sep_iter.Get() : 50;
	int max_kicks = (_kicks)? _kicks.Get() : 30;

	clock_t begin_time=clock();
	auto elapsed = [&]() { return double(clock()-begin_time)/CLOCKS_PER_SEC; };

	try{
		Instance inst = Instance::load(_file.Get(), (_root)? _root.Get() : "", (_dec)? _dec.Get() : "decompositions");
		cout << "instance: " << inst.name << endl;
		cout << "items: " << inst.n_items() << endl;
		cout << "height_lb: " << inst.height_lower_bound() << endl;
		for(const Piece& P : inst.pieces)
			if(!P.convex_parts)
				cout << "WARNING: " << P.name << " no esta descompuesta en partes convexas (se usa la envolvente de cada parte)" << endl;

		Placer placer(inst, grid, slide);
		if(_seed) placer.set_seed(_seed.Get());
		if(_axis_window) placer.set_axis_window(_axis_window.Get());
		int nblocks = (_blocks)? _blocks.Get() : 0;
		if(nblocks > 0){
			placer.generate_blocks(nblocks, (_block_pair)? _block_pair.Get() : 2, (_block_waste)? _block_waste.Get() : 0.07);
			cout << "blocks: " << placer.get_blocks().size() << " (" << elapsed() << " s)" << endl;
		}
		PLP_Function f(placer, alpha, beta, gamma);
		double lb = inst.height_lower_bound();
		LPBackend* lp = (_no_compact)? NULL : LPBackend::create();
		if(_sep_lp && !lp) cout << "WARNING: --sep_lp requiere compilar con SoPlex" << endl;
		mt19937 rng((_seed)? _seed.Get() : 0);

		//BSG con un contenedor de altura hmax (infinito: altura abierta) a partir de las piezas keep ya ubicadas;
		//retorna el mejor estado (o NULL)
		auto search = [&](double hmax, double tl, const vector<Placement>& keep) -> plpState* {
			plpState s0(inst, hmax);
			for(const Placement& p : keep) s0.transition(plpAction(p.type, p.t));
			CompactGreedy gr(&f, NULL);
			BSG bsg(&f, gr, beams);
			DoubleEffort de(bsg);
			if(hmax < numeric_limits<double>::infinity()){
				gr.stop_when_complete.push_back(&gr);
				gr.stop_when_complete.push_back(&bsg);
				gr.stop_when_complete.push_back(&de);
			}
			de.run(*s0.clone(), tl, begin_time);
			const State* best = de.get_best_state();
			return best? static_cast<plpState*>(best->clone()) : NULL;
		};

		//una nueva mejor solucion se acepta solo si es factible (defensa ante errores numericos)
		long rejected = 0;
		auto accept = [&](const Solution& S2) {
			if(S2.verify().feasible) return true;
			rejected++;
			cout << "WARNING: solucion infactible descartada" << endl;
			return false;
		};

		//mejora local y compactacion global por LP
		auto polish = [&](const vector<int>& pieces, vector<Vec3>& t) -> double {
			double H = placer.improve(pieces, t);
			if(lp)
				for(int k=0; k<20; k++){
					placer.compact(pieces, t, *lp);
					double H2 = placer.improve(pieces, t);
					if(H2 >= H - 1e-6*H) { H = min(H, H2); break; }
					H = H2;
				}
			return H;
		};

		//1. construccion (altura abierta), o solucion inicial dada
		plpState* best = NULL;
		if(_start){
			//se carga como un estado completo (las posiciones se reemplazan despues)
			best = new plpState(inst);
		}else if(init=="bsg"){
			best = search(numeric_limits<double>::infinity(), ((_bsg_time)? _bsg_time.Get() : 0.3)*maxtime, vector<Placement>());
			if(!best || best->get_n_left()>0){
				cout << "WARNING: el BSG no completo una solucion; se usa el greedy" << endl;
				delete best;
				best = NULL;
			}
		}
		if(!best){
			best = new plpState(inst);
			CompactGreedy gr(&f, NULL);
			gr.run(*best, 0.0);
		}
		if(!_start && best->get_n_left()>0){
			cerr << "ERROR: no se encontro una solucion completa" << endl;
			return 1;
		}
		Solution S = (_start)? Solution::load(_start.Get(), inst) : best->get_solution();
		delete best;
		vector<int> ps;
		vector<Vec3> ts;
		for(const Placement& p : S.items) { ps.push_back(inst.types[p.type].piece); ts.push_back(p.t); }
		int n = ps.size();
		if(_start && (int)inst.n_items() != n) { cerr << "ERROR: la solucion inicial no tiene todos los items" << endl; return 1; }
		double H = S.height();
		if(_start && !S.verify().feasible){
			//con otra descomposicion la solucion puede tener solapamientos pequenos: se separa subiendo la altura
			map< pair<int,int>, double > w0;
			double h = H;
			for(int k=0; k<50; k++, h *= 1.005){
				vector<Vec3> t2 = ts;
				//todas las piezas dentro del contenedor (con otra descomposicion pueden sobresalir un poco)
				for(int i=0; i<n; i++){
					Vec3 e = inst.pieces[ps[i]].bbox.extent();
					t2[i].x = min(max(t2[i].x, 0.0), max(0.0, inst.W-e.x));
					t2[i].y = min(max(t2[i].y, 0.0), max(0.0, inst.D-e.y));
					t2[i].z = min(max(t2[i].z, 0.0), max(0.0, h-e.z));
				}
				if(placer.minimize_overlap(ps, t2, h, gls_iter, w0, rng, gls=="mult")){
					Solution S2 = S;
					for(int i=0; i<n; i++) S2.items[i].t = t2[i];
					if(S2.verify().feasible) { S.items = S2.items; ts = t2; break; }
				}
			}
			if(!S.verify().feasible) { cerr << "ERROR: no se pudo separar la solucion inicial" << endl; return 1; }
			H = S.height();
			cout << "start_separated_height: " << H << endl;
		}
		cout << "construction_height (" << ((_start)? "start" : init) << ", " << elapsed() << " s): " << H << endl;
		auto set_best = [&](const vector<Vec3>& t, double H2) {
			Solution S2 = S;
			for(int i=0; i<n; i++) S2.items[i].t = t[i];
			if(!accept(S2)) return false;
			S.items = S2.items;
			H = H2;
			return true;
		};
		set_best(ts, polish(ps, ts));
		cout << "compaction_height (" << ((lp)? lp->name() : "sin LP") << "): " << H << endl;

		//2. oscilacion estrategica (Egeblad et al. 2009; Umetani et al. 2009; Gardeyn et al. 2025)
		auto current = [&]() { vector<Vec3> t; for(const Placement& p : S.items) t.push_back(p.t); return t; };
		//piezas dentro del contenedor de altura h (las que sobresalen se bajan aunque se solapen)
		auto clamp = [&](vector<Vec3>& t, double h) {
			for(int i=0; i<n; i++){
				Vec3 e = inst.pieces[ps[i]].bbox.extent();
				t[i].x = min(max(t[i].x, 0.0), max(0.0, inst.W-e.x));
				t[i].y = min(max(t[i].y, 0.0), max(0.0, inst.D-e.y));
				t[i].z = min(max(t[i].z, 0.0), max(0.0, h-e.z));
			}
		};
		long separations = 0;
		int kicks = 0;
		map< pair<int,int>, double > weights;   //pesos de la busqueda local guiada (persisten en la oscilacion)
		double hc = max(lb, H*(1-sep_step));
		ts = current();
		clamp(ts, hc);
		uniform_int_distribution<int> pick(0, n-1);
		while(!_greedy && elapsed() < maxtime && H > lb*(1+1e-6)){
			separations++;
			bool ok = placer.minimize_overlap(ps, ts, hc, gls_iter, weights, rng, gls=="mult");
			if(!ok && _sep_lp && lp) ok = placer.separate(ps, ts, hc, *lp, sep_iter, &weights);
			if(ok){
				double H2 = polish(ps, ts);
				if(H2 < H - 1e-9*H && set_best(ts, H2)){
					cout << "[separacion] (" << elapsed() << " s): altura " << H << " (kicks " << kicks << ")" << endl;
					kicks = 0;
				}
				ts = current();
				hc = max(lb, H*(1-sep_step));
				clamp(ts, hc);
				weights.clear();
				continue;
			}
			//se sube un poco la altura y se sigue desde la solucion solapada
			hc += 0.25*sep_step*H;
			if(hc < H*(1-1e-6)) { clamp(ts, hc); continue; }

			//kick desde la mejor solucion
			kicks++;
			if(_repair && kicks >= max_kicks){
				//reparacion con BSG: se mantienen las piezas bajo (1-band)*hmax y el BSG ubica las demas
				kicks = 0;
				double hmax = max(lb, H*(1-((_step)? _step.Get() : 0.03)));
				double band = (_band)? _band.Get() : 0.2;
				vector<Placement> keep;
				for(const Placement& p : S.items)
					if(p.t.z + inst.pieces[inst.types[p.type].piece].bbox.extent().z <= (1-band)*hmax) keep.push_back(p);
				plpState* s = search(hmax, min((double) maxtime, elapsed() + ((_slice)? _slice.Get() : 0.1)*maxtime), keep);
				if(s && s->get_n_left()==0){
					Solution S2 = s->get_solution();
					vector<Vec3> t2;
					vector<int> p2;
					for(const Placement& p : S2.items) { p2.push_back(inst.types[p.type].piece); t2.push_back(p.t); }
					double H2 = polish(p2, t2);
					for(int i=0; i<n; i++) S2.items[i].t = t2[i];
					cout << "[reparacion] (" << elapsed() << " s): altura " << H2 << endl;
					if(H2 < H && accept(S2)){
						S.items = S2.items;
						H = H2;
						ps = p2;
					}
				}
				delete s;
			}
			ts = current();
			hc = max(lb, H*(1-sep_step));
			clamp(ts, hc);
			int a = pick(rng);
			bool done = false;
			if(kick=="top" || kick=="topbest"){
				//una de las piezas que definen la altura: las que sobresalen de hc en la mejor solucion
				vector<int> top;
				for(int i=0; i<n; i++)
					if(S.items[i].t.z + inst.pieces[ps[i]].bbox.extent().z > hc) top.push_back(i);
				if(!top.empty()) a = top[rng()%top.size()];
			}
			if(kick=="none"){
				done = true;                  //solo se reinicia desde la mejor solucion (pesos en cero)
			}else if(kick=="top" || kick=="topbest"){
				//posicion aleatoria bajo hc; topbest: la de menor penetracion entre kick_samples posiciones
				Vec3 e = inst.pieces[ps[a]].bbox.extent();
				uniform_real_distribution<double> ux(0, max(0.0, inst.W-e.x)), uy(0, max(0.0, inst.D-e.y)), uz(0, max(0.0, hc-e.z));
				int samples = (kick=="topbest")? kick_samples : 1;
				Vec3 best_t = ts[a];
				double best_pen = numeric_limits<double>::max();
				for(int k=0; k<samples; k++){
					ts[a] = Vec3(ux(rng), uy(rng), uz(rng));
					double pen = 0;
					if(samples > 1) for(int b=0; b<n; b++) if(b!=a) pen += placer.pair_penetration(ps, ts, a, b);
					if(pen < best_pen) { best_pen = pen; best_t = ts[a]; }
				}
				ts[a] = best_t;
				done = true;
			}else if(kick=="random" || (kick=="mix" && rng()%2)){
				//posicion aleatoria en el contenedor de altura hc
				Vec3 e = inst.pieces[ps[a]].bbox.extent();
				uniform_real_distribution<double> ux(0, max(0.0, inst.W-e.x)), uy(0, max(0.0, inst.D-e.y)), uz(0, max(0.0, hc-e.z));
				ts[a] = Vec3(ux(rng), uy(rng), uz(rng));
				done = true;
			}else if(kick=="swap"){
				//intercambio de dos piezas de distinto tipo y volumen parecido (razon entre 1/2 y 2)
				vector<int> comp;
				double va = inst.pieces[ps[a]].volume;
				for(int b=0; b<n; b++){
					double vb = inst.pieces[ps[b]].volume;
					if(ps[b]!=ps[a] && vb >= 0.5*va && vb <= 2*va) comp.push_back(b);
				}
				if(!comp.empty()) { placer.swap_pieces(ps, ts, a, comp[rng()%comp.size()], hc); done = true; }
			}
			//reinsercion de una pieza al azar en el espacio libre; si no hay posicion, se intercambian dos piezas
			if(!done && !placer.reinsert(ps, ts, a, hc)){
				int b = pick(rng);
				for(int k=0; k<20 && ps[a]==ps[b]; k++) b = pick(rng);
				if(ps[a]!=ps[b]) swap(ts[a], ts[b]);
			}
			weights.clear();
			clamp(ts, hc);
		}
		//3. separacion final con margen (certificable en aritmetica exacta)
		double margin = (_margin)? _margin.Get() : 1e-7;
		if(lp && margin > 0){
			ts = current();
			bool applied;
			double H2 = placer.compact(ps, ts, *lp, 1, margin, &applied);
			Solution S2 = S;
			for(int i=0; i<n; i++) S2.items[i].t = ts[i];
			if(applied && S2.verify().feasible && H2 <= H + 1e-4*H) { S.items = S2.items; H = S.height(); }
			else cout << "WARNING: no se pudo separar con margen" << endl;
			//las coordenadas se redondean a multiplos de 2^-30 (operaciones exactas): en filas que llenan exactamente
			//el contenedor, las posiciones de contacto suelen ser sumas de dimensiones (representables) y el redondeo
			//elimina el error del LP; el error (< 1e-9) es menor que el margen de los pares separados
			Solution S3 = S;
			const double q = 1.0/(1<<30);
			for(Placement& p : S3.items)
				for(int c=0; c<3; c++) p.t[c] = max(0.0, std::round(p.t[c]/q)*q);
			if(S3.verify().feasible && S3.height() <= H + 1e-6*H) { S.items = S3.items; H = S.height(); }
			cout << "margin_height (" << margin << "): " << H << endl;
		}
		delete lp;

		Report r = S.verify();
		cout << "separations: " << separations << endl;
		cout << "rejected: " << rejected << endl;
		cout << "time: " << elapsed() << endl;
		cout << r;
		if(_out) S.save(_out.Get());
		return r.feasible? 0 : 2;
	}catch(exception& e){
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
