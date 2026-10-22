/*
 * main_plp_check.cpp
 *
 * Muestra una instancia de carga de poliedros y, opcionalmente, verifica una solucion.
 *
 * ejemplo: PLP_check "problems/plp/benchs/Stoyan et al. 2005/Example1.json" -s "problems/plp/benchs/Stoyan et al. 2005/Solution1.json"
 */

#include <iostream>
#include <iomanip>
#include "args.hxx"
#include "Instance.h"
#include "Solution.h"

using namespace std;
using namespace plp;

int main(int argc, char** argv){

	args::ArgumentParser parser("********* PLP check *********.", "Instancias y soluciones del problema de carga de poliedros (formato ESICUP).");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<string> _sol(parser, "file", "Solution to verify (ESICUP JSON)", {'s', "solution"});
	args::ValueFlag<string> _root(parser, "dir", "Root folder of the piece paths (default: searched from the instance folder)", {"root"});
	args::ValueFlag<double> _eps(parser, "double", "Tolerance for overlaps and containment. Default: 1e-6", {"eps"});
	args::Flag _partial(parser, "partial", "Do not require all the items to be placed", {"partial"});
	args::Flag _quiet(parser, "quiet", "Do not print the pieces", {'q', "quiet"});
	args::Positional<std::string> _file(parser, "instance", "Instance (ESICUP JSON)");

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

	double eps = (_eps)? _eps.Get() : 1e-6;

	try{
		Instance inst = Instance::load(_file.Get(), (_root)? _root.Get() : "");

		cout << setprecision(8);
		cout << "instance: " << inst.name << endl;
		cout << "container: " << inst.W << " x " << inst.D << " x (open)" << endl;
		cout << "types: " << inst.types.size() << endl;
		cout << "items: " << inst.n_items() << endl;
		cout << "volume: " << inst.total_volume() << endl;
		cout << "height_lb: " << inst.height_lower_bound() << endl;

		bool decomposed=true;
		for(const ItemType& t : inst.types){
			const Piece& P = inst.pieces[t.piece];
			decomposed &= P.convex_parts;
			Vec3 e = P.bbox.extent();
			if(e.x > inst.W+eps || e.y > inst.D+eps)
				cout << "WARNING: " << P.name << " no cabe en la base del contenedor" << endl;
			if(_quiet) continue;
			int nv=0, nf=0;
			for(const ConvexPolytope& C : P.parts) { nv+=C.get_vertices().size(); nf+=C.get_facets().size(); }
			cout << "  " << t.path << "  x" << t.demand
				 << "  parts:" << P.parts.size() << " vertices:" << nv << " facets:" << nf
				 << "  volume:" << P.volume << "  bbox:" << e
				 << (P.convex? "  convex" : (P.convex_parts? "  union of convex parts" : "  NON-CONVEX (needs decomposition)"))
				 << endl;
			if(!P.parts_file.empty()) cout << "    parts: " << P.parts_file << endl;
			for(const string& w : P.warnings) cout << "    WARNING: " << w << endl;
		}
		cout << "convex_parts: " << (decomposed? "yes":"no") << endl;

		if(_sol){
			Solution S = Solution::load(_sol.Get(), inst);
			Report r = S.verify(eps, _partial);
			cout << "***** Solution: " << _sol.Get() << " *****" << endl;
			cout << r;
			return r.feasible? 0 : 2;
		}
	}catch(exception& e){
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
	return 0;
}
