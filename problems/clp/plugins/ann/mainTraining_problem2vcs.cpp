/*
 * gen_ran_inst_solve.cpp
 *
 * Generate a random instance and solves it!
 *
 *  Created on: 18 dic. 2017
 *      Author: iaraya
 */


#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
//#include "objects/State.cpp"
#include "clpState.h"
#include "clpStatekd.h"
#include "BlockSet.h"
#include "BSG_midBSG.h"
#include "VCS_Function.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"
#include "args.hxx"
#include "random_instances_generator.hpp"

bool global::TRACE = false;

using namespace std;

int greedy_eval(Greedy *gr, VCS_Function* vcs, clpState* s0){

	double val=0.0;
	int best_conf=-1;
	for (int i=0;i<999;i++){
		State& s_copy= *s0->clone();
		list<State*> S;
		S.push_back(&s_copy);
		vcs->use_configuration(i);

		while(!S.empty())
			gr->next(S);
		if(s_copy.get_value() > val){
		   val=s_copy.get_value();
			 best_conf=i;
		 }
	}
	cout << best_conf << endl;
	return best_conf;
}

double random(double min, double max){
	return min + ((double)rand()/(double)RAND_MAX)*(max-min);
}

void generate_data(string file, double min_fr){

	Space::bottom_up=false;
	int inst=0;

	//cout << file<<endl;
  //cout << "cargando la instancia..." << endl;

    Block::FSB=false;
    clpState* s0 = new_state(file,inst, min_fr, 10000);

  //  cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    double alpha= random(0,8);
	double beta= random(0,8);

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, 0.2, 0.04, 1.0, 0.0, 0.0);

		for(int i=0; i<1000; i++){
			double alpha= random(0,8);
			double beta= random(0,8);

			vcs->add_configuration(alpha, beta, 0.2, 0.04, 1.0);
		}

      Greedy *gr = new Greedy (vcs);

	  list<State*> S;
	  S.push_back(s0);
	  compactState c(*s0);
	  int best_conf=greedy_eval(gr, vcs, s0);
	  vcs->use_configuration(best_conf);

    cout << c << endl;
	  cout << vcs->get_alpha() << "," << vcs->get_beta() << endl;

}

int main(int argc, char** argv) {
	args::ArgumentParser parser("********* CLP Instance Generator (and solver) *********.", "Generates a random instance and solves it.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> L(parser, "int", "Length of the container", {'L'});
	args::ValueFlag<int> W(parser, "int", "Width of the container", {'W'});
	args::ValueFlag<int> H(parser, "int", "Height of the container", {'H'});
	args::ValueFlag<int> N(parser, "int", "Number of box types", {'N'});
	args::ValueFlag<int> min_l(parser, "int", "Minimum length", {"minl"});
	args::ValueFlag<int> min_w(parser, "int", "Minimum width", {"minw"});
	args::ValueFlag<int> min_h(parser, "int", "Minimum height", {"minh"});
	args::ValueFlag<int> max_l(parser, "int", "Maximum length", {"maxl"});
	args::ValueFlag<int> max_w(parser, "int", "Maximum width", {"maxw"});
	args::ValueFlag<int> max_h(parser, "int", "Maximum height", {"maxh"});
	args::ValueFlag<double> bsl(parser, "int", "Box stability limit", {"bsl"});
	args::ValueFlag<int> s(parser, "int", "Generator seed", {'s'});

	args::ValueFlag<double> min_fr(parser, "double", "Minimum volume of a block", {"min_fr"});

	args::ValueFlag<int> maxtime(parser, "int", "Max Time", {"maxtime"});

	args::ValueFlag<std::string> folder(parser, "string", "The path of the work folder", {'f'});
	args::Flag _solve(parser, "solve", "Solve the generated instance", {"solve"});


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

	int _L=587, _W=233, _H=220;
	if(L) _L=L.Get();
	if(W) _W=W.Get();
	if(H) _H=H.Get();

	int _N=100;
	if(N) _N=N.Get();

	int _min_l=30, _min_w=25, _min_h=20;
	int _max_l=120, _max_w=100, _max_h=80;
	if(min_l) _min_l=min_l.Get();
	if(min_w) _min_w=min_w.Get();
	if(min_h) _min_h=min_h.Get();
	if(max_l) _max_l=max_l.Get();
	if(max_w) _max_w=max_w.Get();
	if(max_h) _max_h=max_h.Get();

	double _bsl=2.0;
	if(bsl) _bsl=bsl.Get();

	int _s=1;
	if(s) _s=s.Get();
	srand(_s);

	double _min_fr=(_N>25)? 0.98:1.0;
	int _maxtime=2;
	if(min_fr) _min_fr=min_fr.Get();


	string f="";

	if(folder) f=folder.Get();

	char mkdir[128];
	strcpy(mkdir,"mkdir ");
	strcat(mkdir,f.c_str());
	//int i=system(mkdir);

	string file = f + "tmp_" + std::to_string(_N) + "_" + std::to_string(_s);

	string output_file1 =f + "tsample_block_" + std::to_string(_N) + "_" + std::to_string(_s);
	string output_file2 =f + "tsample_box_" + std::to_string(_N) + "_" + std::to_string(_s);

	//cout << file << endl;
	//cout << output_file1 << endl;
	//cout << output_file2 << endl;
	//cout << _N << endl;

	generaInstancia(_L, _W, _H, _N, _min_l, _min_w, _min_h, _max_l, _max_w, _max_h, _bsl, _s, file.c_str());
	//cout << f << "/" << file << endl;
	if(_solve){
		generate_data(file, _min_fr);
		char rm[128];
		strcpy(rm,"rm ");
		strcat(rm,file.c_str());
		int i=system(rm);
	}



}
