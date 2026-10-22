/*
 * main_plp.cpp
 *
 * BSG para la carga de poliedros (3D irregular strip packing, sin rotaciones)
 *
 * ejemplo: BSG_PLP "problems/plp/benchs/Stoyan et al. 2005/Example1.json" -t 30 -o sol.json
 */

#include <iostream>
#include "args.hxx"
#include "plpState.h"
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
	args::ValueFlag<double> _beta(parser, "double", "Weight of the height of the piece. Default: 1", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Weight of the increase of the height. Default: 1", {"gamma"});
	args::ValueFlag<int> _grid(parser, "int", "Starting points per axis (x,y). Default: 3", {"grid"});
	args::ValueFlag<double> _slide(parser, "double", "Weight of x,y (towards the walls) in the LP descent. Default: 0.1", {"slide"});
	args::ValueFlag<int> _beams(parser, "int", "Initial number of beams. Default: 4", {"beams"});
	args::Flag _greedy(parser, "greedy", "Only the greedy algorithm", {"greedy"});
	args::Flag _no_improve(parser, "no_improve", "Do not improve the best solution (compaction and relocation of the highest pieces)", {"no_improve"});
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
	double beta = (_beta)? _beta.Get() : 1.0;
	double gamma = (_gamma)? _gamma.Get() : 1.0;
	int grid = (_grid)? _grid.Get() : 3;
	double slide = (_slide)? _slide.Get() : 0.1;
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
		PLP_Function f(placer, alpha, beta, gamma);
		plpState s0(inst);

		const plpState* best;
		Greedy gr(&f);
		BSG bsg(&f, gr, beams);
		DoubleEffort de(bsg);
		if(_greedy){
			gr.run(s0, maxtime, begin_time);
			best = &s0;
		}else{
			State& s = *s0.clone();
			de.run(s, maxtime, begin_time);
			best = static_cast<const plpState*>(de.get_best_state());
		}
		if(!best || best->get_n_left()>0){
			cerr << "ERROR: no se encontro una solucion completa" << endl;
			return 1;
		}

		Solution S = best->get_solution();
		cout << "search_height: " << S.height() << endl;
		if(!_no_improve){
			vector<int> pieces;
			vector<Vec3> t;
			for(const Placement& p : S.items) { pieces.push_back(inst.types[p.type].piece); t.push_back(p.t); }
			placer.improve(pieces, t);
			for(size_t i=0; i<t.size(); i++) S.items[i].t = t[i];
		}

		double time = double(clock()-begin_time)/CLOCKS_PER_SEC;
		Report r = S.verify();
		cout << "time: " << time << endl;
		cout << r;
		if(_out) S.save(_out.Get());
		return r.feasible? 0 : 2;
	}catch(exception& e){
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
}
