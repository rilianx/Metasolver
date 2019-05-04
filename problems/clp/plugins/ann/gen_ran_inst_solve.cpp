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

void solve(string file, string path1, string path2, int _N, double min_fr, int max_time, double alpha, double beta, double gamma, double p,
		double delta=1.0, double f=0.0, double r=0.0, bool fsb=false, bool kdtree=false){

	Space::bottom_up=true;
	int inst=0;

	cout << file<<endl;
    cout << "cargando la instancia..." << endl;


    Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, 10000);

    if(kdtree)
       s0 = new clpState_kd(*s0);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, f, r);

	if(kdtree){
		kd_block::set_vcs(*vcs);
		kd_block::set_alpha(alpha);
		kd_block::set_alpha(p);
	}


	cout << "greedy" << endl;
    Greedy *gr = new Greedy (vcs);

	list<State*> S;
	S.push_back(s0);
	for(int i=0;i<0;i++)
		gr->next(S);

	compactState c(*s0);


	exit(0);

	cout << "bsg" << endl;
    BSG *bsg= new BSG(vcs,*gr, 4);
    //BSG_midBSG *bsg= new BSG_midBSG(*gr, *exp, 4);

    //bsg->set_shuffle_best_path(true);

	cout << "double effort" << endl;
    SearchStrategy *de= new DoubleEffort(*bsg);

	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

   // cout << s0.valid_blocks.size() << endl;

	cout << "running" << endl;
    double eval = 1-de->run(s_copy, max_time, begin_time) ;
	cout << 1-eval << endl;


	stringstream ss;
	ss << inst;
	string indiceInstancia = ss.str();

	//cout <<"indice instancia"<<indiceInstancia<<endl;

	list<const Action*>& actions= dynamic_cast<const clpState*>(de->get_best_state())->get_path();

	clpState* s00 = dynamic_cast<clpState*> (s0->clone());

	const AABB* aux= &s00->cont->blocks->top();
	int width=587,length=233, maxval=220;

	ofstream myfile;
	ofstream myfile1;

	myfile.open(path1);
	myfile1.open(path2);

	myfile << eval <<endl;

	myfile1 << eval <<endl;

	int n_tipos = _N;

	addHeader( file,  n_tipos, myfile);
	addHeader( file,  n_tipos, myfile1);


	for(auto action:actions){

		const clpAction* clp_action = dynamic_cast<const clpAction*> (action);

		const AABB* aux= &s00->cont->blocks->top();

		s00->transition(*clp_action);


		//cout << "block :" << clp_action->block << endl;

		list< AABB > uni_blocks;
		AABB aabb(clp_action->space.get_location(clp_action->block),&clp_action->block);
		extract_boxes(aabb, uni_blocks);

		uni_blocks.sort(compare_aabb);

		for(auto aabb:uni_blocks){
			//cout << aabb << endl;
			myfile1 << aabb <<endl;
		}


		myfile << clp_action->block << " ; ";
		//cout << "location :" << clp_action->space.get_location(clp_action->block) << endl;
		myfile << clp_action->space.get_location(clp_action->block) << endl;

		for(auto box : clp_action->block.nb_boxes){
			if(box.second > 0) myfile << box.first->get_id() << "," << box.second << " ";
		}
		myfile << endl;

	}
	myfile.close();
	myfile1.close();

	//s00->cont->MatLab_print();


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
	args::ValueFlag<double> alpha(parser, "double", "Parameter alpha of VCS", {"alpha"});
	args::ValueFlag<double> beta(parser, "double", "Parameter beta of VCS", {"beta"});
	args::ValueFlag<double> gamma(parser, "double", "Parameter gamma of VCS", {"gamma"});
	args::ValueFlag<double> p(parser, "double", "Parameter p of VCS", {'p'});
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

	double _min_fr=0.98;
	int _maxtime=2;
	double _alpha=4.0, _beta=1.0, _gamma=0.2, _p=0.04;
	if(maxtime) _maxtime=maxtime.Get();
	if(alpha) _alpha=alpha.Get();
	if(beta) _beta=beta.Get();
	if(gamma) _gamma=gamma.Get();
	if(p) _p=p.Get();


	string f="";

	if(folder) f=folder.Get();

	char mkdir[128];
	strcpy(mkdir,"mkdir ");
	strcat(mkdir,f.c_str());
	int i=system(mkdir);

	string file = f + "tmp_" + std::to_string(_N) + "_" + std::to_string(_s);

	string output_file1 =f + "tsample_block_" + std::to_string(_N) + "_" + std::to_string(_s);
	string output_file2 =f + "tsample_box_" + std::to_string(_N) + "_" + std::to_string(_s);

	cout << file << endl;
	cout << output_file1 << endl;
	cout << output_file2 << endl;
	cout << _N << endl;

	generaInstancia(_L, _W, _H, _N, _min_l, _min_w, _min_h, _max_l, _max_w, _max_h, _bsl, _s, file.c_str());
	cout << f << "/" << file << endl;
	if(_solve){
		solve(file, output_file1, output_file2, _N, _min_fr, _maxtime, _alpha, _beta, _gamma, _p);
		char rm[128];
		strcpy(rm,"rm ");
		strcat(rm,file.c_str());
		int i=system(rm);
	}



}
