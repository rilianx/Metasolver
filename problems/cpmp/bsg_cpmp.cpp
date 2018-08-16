#include <iostream>
#include "args.hxx"

#include "cpmpState.h"
#include "cpmpEvaluator.h"
#include "Greedy.h"
#include "BSG.h"

using namespace std;

using namespace metasolver;
using namespace cpmp;

int main(int argc, char** argv){


	args::ArgumentParser parser("********* A* CPMP *********.", "A* for the CPMP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::Flag trace(parser, "trace", "Trace", {"trace"});
	args::Positional<std::string> _file(parser, "instance-set", "The name of the instance set");

	cout.precision(8);
	try
	{
		parser.ParseCLI(argc, argv);

	}
	catch (args::Help&)
	{
		std::cout << parser;
		return 0;
	}
	catch (args::ParseError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	catch (args::ValidationError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}


	cpmpState* s0 = new cpmpState(_file.Get());
	s0->print();


	clock_t begin_time=clock();

	cpmpEvaluator* ev = new cpmpEvaluator();


	cout << "greedy" << endl;
	SearchStrategy *gr = new Greedy (ev, 9);

	gr->run(*s0);

	//cout << "bsg" << endl;
	//BSG *bsg= new BSG(ev,*gr, 10);

	gr->get_best_state()->print();




}
