/*
 * main_plp.cpp
 *
 * BSG para la carga de poliedros (3D irregular strip packing, sin rotaciones)
 *
 * ejemplo: BSG_PLP "problems/plp/benchs/Stoyan et al. 2005/Example1.json" -t 30 -o sol.json
 */

#include <iostream>
#include <random>
#include <algorithm>
#include "args.hxx"
#include "plpState.h"
#include "LPBackend.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "BSG.h"

using namespace std;
using namespace plp;

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-PLP *********.", "BSG Solver for 3D irregular strip packing (polyhedra, ESICUP format).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit (seconds). Default: 30", {'t', "timelimit"});
	args::ValueFlag<double> _alpha(parser, "double", "Weight of the volume of the piece. Default: 1", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Weight of the height of the piece. Default: 5.53", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Weight of the increase of the height. Default: 0.77", {"gamma"});
	args::ValueFlag<int> _grid(parser, "int", "Starting points per axis (x,y). Default: 4", {"grid"});
	args::ValueFlag<double> _slide(parser, "double", "Weight of x,y (towards the walls) in the LP descent. Default: 0.065", {"slide"});
	args::ValueFlag<int> _beams(parser, "int", "Initial number of beams. Default: 4", {"beams"});
	args::Flag _no_separate(parser, "no_separate", "Fixed height: do not try the LP separation (requires ibex) before the BSG repair", {"no_separate"});
	args::ValueFlag<double> _sep_step(parser, "double", "Initial height decrease of the LP separation phase (proportion). Default: 0.01", {"sep_step"});
	args::ValueFlag<string> _sep_method(parser, "axis|lp|both", "Separation: minimum-overlap moves along the axes with guided local search (axis), LP separation (lp) or both (axis, then lp). Default: both", {"sep_method"});
	args::ValueFlag<int> _gls_iter(parser, "int", "Maximum passes of the minimum-overlap local search. Default: 500", {"gls_iter"});
	args::ValueFlag<int> _kick_size(parser, "int", "Kick: maximum number of pieces reinserted (uniform between 1 and this value). Default: 1", {"kick_size"});
	args::ValueFlag<int> _kicks(parser, "int", "Separation phase: consecutive kicks without improvement before trying the BSG repair. Default: 30", {"kicks"});
	args::ValueFlag<string> _gls(parser, "add|mult", "Guided local search weights: +1 for the overlapping pairs (add) or multiplicative by severity with decay (mult). Default: mult", {"gls"});
	args::Flag _fls(parser, "fls", "Fast local search in the minimum-overlap search: only the pieces whose neighborhood changed are moved", {"fls"});
	args::ValueFlag<int> _strikes(parser, "int", "Minimum-overlap search: passes without improving the best overlapped layout before going back to it (0: never); 3 strikes end the search. Default: 0", {"strikes"});
	args::ValueFlag<double> _sep_inc(parser, "double", "Height increase after a failed separation (proportion of sep_step; 0: kick directly). Default: 0.25", {"sep_inc"});
	args::ValueFlag<string> _kick(parser, "reinsert|swap|mix", "Kick: reinsert pieces in the free space, swap two pieces of different type and similar volume, or either (1/2 each). Default: reinsert", {"kick"});
	args::ValueFlag<double> _compress(parser, "double", "Proportion of the time for a final compression phase: from the best solution, small height decreases (0.05% to 0.001%) and separation. Default: 0", {"compress"});
	args::ValueFlag<int> _sep_iter(parser, "int", "Maximum iterations of the LP separation. Default: 50", {"sep_iter"});
	args::ValueFlag<unsigned> _seed(parser, "int", "Seed (0: no perturbation of the starting points). Default: 0", {"seed"});
	args::ValueFlag<int> _blocks(parser, "int", "Maximum number of pseudo-blocks (pairs of pieces in a compact relative position). Default: 30", {"blocks"});
	args::ValueFlag<int> _block_pair(parser, "int", "Pseudo-blocks per pair of item types. Default: 2", {"block_pair"});
	args::ValueFlag<string> _block_criterion(parser, "box|hull", "Acceptance of pseudo-blocks: reduction of the bounding box (box) or added waste in the convex hull (hull). Default: hull", {"block_criterion"});
	args::ValueFlag<double> _block_gain(parser, "double", "box criterion: minimum reduction of the bounding box w.r.t. the boxes of the pieces. Default: 0.05", {"block_gain"});
	args::ValueFlag<string> _block_eval(parser, "total|piece", "Volume of a pseudo-block in the evaluation: total or per piece. Default: total", {"block_eval"});
	args::ValueFlag<double> _block_waste(parser, "double", "Maximum added waste of a pseudo-block: (convex hull of the block - convex hulls of the pieces) / volume of the pieces. Default: 0.07", {"block_waste"});
	args::Flag _greedy(parser, "greedy", "Only the greedy algorithm", {"greedy"});
	args::Flag _no_improve(parser, "no_improve", "Do not improve the best solution (compaction and relocation of the highest pieces)", {"no_improve"});
	args::Flag _no_compact(parser, "no_compact", "Do not use the global LP compaction (requires ibex)", {"no_compact"});
	args::ValueFlag<int> _compact_greedy(parser, "int", "Compact every greedy solution with this number of LP iterations (0: only the best solution). Default: 0", {"compact_greedy"});
	args::ValueFlag<double> _strip_time(parser, "double", "Proportion of the time for the strip packing search; the rest decreases the height and solves the problem with a fixed container (knapsack). Default: 0.3 (1: no decrease)", {"strip_time"});
	args::ValueFlag<double> _step(parser, "double", "Initial height decrease (proportion of the height). Default: 0.03", {"step"});
	args::ValueFlag<double> _slice(parser, "double", "Maximum time (proportion of the timelimit) for each fixed height. Default: 0.1", {"slice"});
	args::ValueFlag<double> _band(parser, "double", "Fixed height: the pieces of the best solution below (1-band)*H are kept and the BSG places the rest; on failure the band grows (x1.5) up to 1 (from scratch). Default: 0.2", {"band"});
	args::ValueFlag<string> _out(parser, "file", "Output solution (ESICUP JSON)", {'o', "output"});
	args::ValueFlag<string> _root(parser, "dir", "Root folder of the piece paths", {"root"});
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
	double alpha = (_alpha)? _alpha.Get() : 1.0;
	//ajustados con Optuna (problems/plp/results/ajuste.md)
	double beta = (_beta)? _beta.Get() : 5.53;
	double gamma = (_gamma)? _gamma.Get() : 0.77;
	int grid = (_grid)? _grid.Get() : 4;
	double slide = (_slide)? _slide.Get() : 0.065;
	int beams = (_beams)? _beams.Get() : 4;

	clock_t begin_time=clock();

	try{
		Instance inst = Instance::load(_file.Get(), (_root)? _root.Get() : "");
		cout << "instance: " << inst.name << endl;
		cout << "items: " << inst.n_items() << endl;
		cout << "height_lb: " << inst.height_lower_bound() << endl;
		for(const Piece& P : inst.pieces)
			if(!P.convex_parts)
				cout << "WARNING: " << P.name << " no esta descompuesta en partes convexas (se usa la envolvente de cada parte)" << endl;

		Placer placer(inst, grid, slide);
		if(_seed) placer.set_seed(_seed.Get());
		int nblocks = (_blocks)? _blocks.Get() : 30;
		if(nblocks > 0){
			string crit = (_block_criterion)? _block_criterion.Get() : "hull";
			if(crit!="box" && crit!="hull") throw runtime_error("--block_criterion debe ser box o hull");
			placer.generate_blocks(nblocks, (_block_pair)? _block_pair.Get() : 2, (_block_waste)? _block_waste.Get() : 0.07, 30,
					crit=="box", (_block_gain)? _block_gain.Get() : 0.05);
			cout << "blocks: " << placer.get_blocks().size() << " (" << double(clock()-begin_time)/CLOCKS_PER_SEC << " s)" << endl;
			for(const Block& B : placer.get_blocks())
				cout << "  " << inst.pieces[inst.types[B.k1].piece].name << " + " << inst.pieces[inst.types[B.k2].piece].name
					 << "  waste " << B.waste << "  density " << B.density << "  bbox " << B.bbox.extent() << endl;
		}
		string beval = (_block_eval)? _block_eval.Get() : "total";
		if(beval!="total" && beval!="piece") throw runtime_error("--block_eval debe ser total o piece");
		PLP_Function f(placer, alpha, beta, gamma, beval=="piece");
		int cg = (_compact_greedy)? _compact_greedy.Get() : 0;
		double strip_time = (_strip_time)? _strip_time.Get() : 0.3;
		double step = (_step)? _step.Get() : 0.03;
		double slice = ((_slice)? _slice.Get() : 0.1) * maxtime;
		double lb = inst.height_lower_bound();
		auto elapsed = [&]() { return double(clock()-begin_time)/CLOCKS_PER_SEC; };

		LPBackend* lpg = (cg>0 && !_no_compact)? LPBackend::create() : NULL;
		if(cg>0 && !lpg) cout << "WARNING: --compact_greedy requiere compilar con ibex" << endl;
		LPBackend* lp = (_no_compact)? NULL : LPBackend::create();

		//BSG con un contenedor de altura hmax (infinito: altura abierta) a partir de las piezas keep ya ubicadas;
		//retorna el mejor estado (o NULL)
		auto search = [&](double hmax, double tl, const vector<Placement>& keep) -> plpState* {
			plpState s0(inst, hmax);
			for(const Placement& p : keep) s0.transition(plpAction(p.type, p.t));
			CompactGreedy gr(&f, lpg, cg);
			BSG bsg(&f, gr, beams);
			DoubleEffort de(bsg);
			if(hmax < numeric_limits<double>::infinity()){
				gr.stop_when_complete.push_back(&gr);
				gr.stop_when_complete.push_back(&bsg);
				gr.stop_when_complete.push_back(&de);
			}
			const State* best;
			if(_greedy){
				gr.run(s0, tl, begin_time);
				best = &s0;
			}else{
				de.run(*s0.clone(), tl, begin_time);
				best = de.get_best_state();
			}
			return best? static_cast<plpState*>(best->clone()) : NULL;
		};

		//mejora local y compactacion global por LP
		auto polish = [&](vector<int>& pieces, vector<Vec3>& t) -> double {
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

		//1. strip packing (altura abierta)
		plpState* best = search(numeric_limits<double>::infinity(), (_greedy)? maxtime : strip_time*maxtime, vector<Placement>());
		if(!best || best->get_n_left()>0){
			//la busqueda no alcanzo a completar una solucion: greedy sin limite de tiempo
			cout << "WARNING: sin solucion completa en la busqueda inicial; se completa con un greedy" << endl;
			delete best;
			best = new plpState(inst);
			CompactGreedy gr(&f, lpg, cg);
			gr.run(*best, 0.0);
		}
		if(best->get_n_left()>0){
			cerr << "ERROR: no se encontro una solucion completa" << endl;
			return 1;
		}
		Solution S = best->get_solution();
		delete best;
		vector<int> pieces;
		vector<Vec3> t;
		for(const Placement& p : S.items) { pieces.push_back(inst.types[p.type].piece); t.push_back(p.t); }
		cout << "search_height: " << S.height() << endl;
		double H = S.height();
		if(!_no_improve){
			H = polish(pieces, t);
			for(size_t i=0; i<t.size(); i++) S.items[i].t = t[i];
			cout << "improve_height (" << ((lp)? lp->name() : "sin LP") << "): " << H << endl;
		}

		//2. se baja la altura y se resuelve con un contenedor de altura fija
		double band0 = (_band)? _band.Get() : 0.2, band = band0;
		double sep_step = (_sep_step)? _sep_step.Get() : 0.01;
		bool need_sep = true;
		int max_kicks = (_kicks)? _kicks.Get() : 30;
		int kick_size = (_kick_size)? _kick_size.Get() : 1;
		string sep_method = (_sep_method)? _sep_method.Get() : "both";
		if(sep_method!="axis" && sep_method!="lp" && sep_method!="both") throw runtime_error("--sep_method debe ser axis, lp o both");
		int gls_iter = (_gls_iter)? _gls_iter.Get() : 500;
		mt19937 rng((_seed)? _seed.Get() : 0);
		string gls = (_gls)? _gls.Get() : "mult";
		if(gls!="add" && gls!="mult") throw runtime_error("--gls debe ser add o mult");
		int strikes = (_strikes)? _strikes.Get() : 0;
		double sep_inc = (_sep_inc)? _sep_inc.Get() : 0.25;
		string kick = (_kick)? _kick.Get() : "reinsert";
		if(kick!="reinsert" && kick!="swap" && kick!="mix") throw runtime_error("--kick debe ser reinsert, swap o mix");
		double compress = (_compress)? _compress.Get() : 0.0;
		//fin de la exploracion (el resto del tiempo es para la compresion)
		double t_explore = maxtime*(1-compress);
		long sep_calls = 0;
		auto min_overlap = [&](const vector<int>& ps, vector<Vec3>& ts, double h, map< pair<int,int>, double >& w) {
			sep_calls++;
			return placer.minimize_overlap(ps, ts, h, gls_iter, w, rng, gls=="mult", strikes, 3, _fls);
		};
		while(!_greedy && strip_time < 1.0 && elapsed() < t_explore && H > lb*(1+1e-6) && (step > 1e-4 || (need_sep && lp && !_no_separate))){
			double hmax = max(lb, H*(1-step));
			double tl = min(t_explore, elapsed() + slice);

			//fase A: oscilacion estrategica con separacion por LP (Egeblad et al. 2009; Lamas-Fernandez et al. 2023).
			//Se baja la altura (sep_step) y la separacion elimina el solapamiento: si lo logra, se mejora la solucion y
			//se vuelve a bajar; si no, se sube un poco la altura (sep_step/4) y se sigue desde la solucion solapada.
			//Cuando la altura vuelve a la de la mejor solucion, kick: se intercambian dos piezas de la mejor solucion.
			//La fase termina tras max_kicks kicks seguidos sin mejora; luego un intento de reparacion con BSG.
			if(lp && !_no_separate && need_sep){
				need_sep = false;
				int sep_iter = (_sep_iter)? _sep_iter.Get() : 50;
				vector<int> ps;
				for(const Placement& p : S.items) ps.push_back(inst.types[p.type].piece);
				auto ext = [&](int i) { return inst.pieces[ps[i]].bbox.extent(); };
				//piezas dentro del contenedor de altura h (las que sobresalen se bajan aunque se solapen)
				auto clamp = [&](vector<Vec3>& ts, double h) {
					for(size_t i=0; i<ts.size(); i++){
						Vec3 e = ext(i);
						ts[i].x = min(max(ts[i].x, 0.0), max(0.0, inst.W-e.x));
						ts[i].y = min(max(ts[i].y, 0.0), max(0.0, inst.D-e.y));
						ts[i].z = min(max(ts[i].z, 0.0), max(0.0, h-e.z));
					}
				};
				vector<Vec3> ts;
				for(const Placement& p : S.items) ts.push_back(p.t);
				double hc = max(lb, H*(1-sep_step));
				clamp(ts, hc);
				int kicks = 0, n = ps.size();
				map< pair<int,int>, double > weights;   //pesos de la busqueda local guiada (persisten en la oscilacion)
				while(kicks < max_kicks && elapsed() < t_explore && H > lb*(1+1e-6)){
					bool sep_ok = false;
					if(sep_method != "lp") sep_ok = min_overlap(ps, ts, hc, weights);
					if(!sep_ok && sep_method != "axis") sep_ok = placer.separate(ps, ts, hc, *lp, sep_iter, &weights);
					if(sep_ok){
						double H2 = (_no_improve)? hc : polish(ps, ts);
						if(H2 < H - 1e-9*H){
							H = H2;
							for(int i=0; i<n; i++) S.items[i].t = ts[i];
							cout << "[separacion] (" << elapsed() << " s): altura " << H << " (kicks " << kicks << ")" << endl;
							kicks = 0;
						}
						hc = max(lb, min(H, H2)*(1-sep_step));
						clamp(ts, hc);
						weights.clear();
						continue;
					}
					//oscilacion: se sube un poco la altura y se sigue desde la solucion solapada
					hc += sep_inc*sep_step*H;
					if(sep_inc > 0 && hc < H*(1-1e-6)) { clamp(ts, hc); continue; }

					//kick: desde la mejor solucion se reinserta una pieza al azar en el espacio libre (altura H*(1-sep_step));
					//si no hay posicion, se intercambian dos piezas de distinto tipo
					kicks++;
					ts.clear();
					for(const Placement& p : S.items) ts.push_back(p.t);
					hc = max(lb, H*(1-sep_step));
					clamp(ts, hc);
					uniform_int_distribution<int> pick(0, n-1);
					if(kick=="swap" || (kick=="mix" && rng()%2)){
						//intercambio de dos piezas de distinto tipo y volumen parecido (razon entre 1/2 y 2)
						int a = pick(rng);
						vector<int> comp;
						double va = inst.pieces[ps[a]].volume;
						for(int b=0; b<n; b++){
							double vb = inst.pieces[ps[b]].volume;
							if(ps[b]!=ps[a] && vb >= 0.5*va && vb <= 2*va) comp.push_back(b);
						}
						if(!comp.empty()){
							placer.swap_pieces(ps, ts, a, comp[rng()%comp.size()], hc);
							weights.clear();
							continue;
						}
					}
					//se reinsertan entre 1 y kick_size piezas distintas, una tras otra
					int nk = uniform_int_distribution<int>(1, max(1, min(kick_size, n)))(rng);
					vector<int> chosen;
					while((int)chosen.size() < nk){
						int a = pick(rng);
						if(find(chosen.begin(), chosen.end(), a)==chosen.end()) chosen.push_back(a);
					}
					for(int a : chosen)
						if(!placer.reinsert(ps, ts, a, hc) && nk==1){
							int b = pick(rng);
							for(int k=0; k<20 && ps[a]==ps[b]; k++) b = pick(rng);
							if(ps[a]!=ps[b]) swap(ts[a], ts[b]);
						}
					weights.clear();
					hc = max(lb, H*(1-sep_step));
					clamp(ts, hc);
				}
				continue;
			}

			//se mantienen las piezas de la mejor solucion bajo (1-band)*hmax
			vector<Placement> keep;
			for(const Placement& p : S.items)
				if(band < 1.0 && p.t.z + inst.pieces[inst.types[p.type].piece].bbox.extent().z <= (1-band)*hmax) keep.push_back(p);
			plpState* s = search(hmax, tl, keep);
			if(!s) break;
			bool ok = s->get_n_left()==0;
			cout << "[knapsack] H=" << hmax << " band=" << min(band,1.0) << " fijas=" << keep.size() << " (" << elapsed() << " s): ";
			if(ok) band = band0;
			else{
				cout << s->get_value()*100 << "% del volumen; ";
				//mas piezas libres; si ya se partio de cero, se baja menos la altura
				if(band >= 1.0) { step /= 2; band = band0; }
				else band *= 1.5;
				//se completa la carga parcial sobre la carga (altura abierta)
				s->open_height();
				CompactGreedy gr(&f, lpg, cg);
				gr.run(*s, 0.0);
			}
			if(s->get_n_left()==0){
				Solution S2 = s->get_solution();
				vector<int> p2;
				vector<Vec3> t2;
				for(const Placement& p : S2.items) { p2.push_back(inst.types[p.type].piece); t2.push_back(p.t); }
				double H2 = (_no_improve)? S2.height() : polish(p2, t2);
				for(size_t i=0; i<t2.size(); i++) S2.items[i].t = t2[i];
				cout << ((ok)? "todas las piezas" : "completada") << ", altura " << H2 << endl;
				if(H2 < H) { H = H2; S.items = S2.items; need_sep = true; }
			}else cout << endl;
			delete s;
			need_sep = true;
		}

		//3. compresion (Gardeyn et al. 2025): desde la mejor solucion se baja la altura en pasos pequenos, que
		//disminuyen con el tiempo (0.05% a 0.001%), y se separa; si falla se vuelve a la mejor solucion
		if(compress > 0 && strip_time < 1.0 && !_greedy){
			vector<int> ps;
			for(const Placement& p : S.items) ps.push_back(inst.types[p.type].piece);
			int n = ps.size(), tries = 0, ok = 0;
			while(elapsed() < maxtime && H > lb*(1+1e-6)){
				double frac = min(1.0, max(0.0, (elapsed()-t_explore)/max(1e-9, maxtime-t_explore)));
				double cstep = 5e-4*(1-frac) + 1e-5*frac;
				double hc = max(lb, H*(1-cstep));
				vector<Vec3> ts;
				for(const Placement& p : S.items) ts.push_back(p.t);
				for(int i=0; i<n; i++){
					Vec3 e = inst.pieces[ps[i]].bbox.extent();
					ts[i].z = min(max(ts[i].z, 0.0), max(0.0, hc-e.z));
				}
				map< pair<int,int>, double > weights;
				bool sep_ok = false;
				if(sep_method != "lp") sep_ok = min_overlap(ps, ts, hc, weights);
				if(!sep_ok && lp && sep_method != "axis") sep_ok = placer.separate(ps, ts, hc, *lp, (_sep_iter)? _sep_iter.Get() : 50, &weights);
				tries++;
				if(sep_ok){
					double H2 = (_no_improve)? hc : polish(ps, ts);
					if(H2 < H - 1e-12*H){
						H = H2;
						for(int i=0; i<n; i++) S.items[i].t = ts[i];
						ok++;
					}
				}
			}
			cout << "[compresion] altura " << H << " (" << ok << "/" << tries << " intentos exitosos)" << endl;
		}
		delete lpg;
		delete lp;

		double time = double(clock()-begin_time)/CLOCKS_PER_SEC;
		Report r = S.verify();
		cout << "separations: " << sep_calls << endl;
		cout << "time: " << time << endl;
		cout << r;
		if(_out) S.save(_out.Get());
		return r.feasible? 0 : 2;
	}catch(exception& e){
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
