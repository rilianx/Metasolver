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

double greedy_eval(Greedy *g, Greedy *gr, clpState* s0){

	double val=0.0;
	for (int i=0;i<100;i++){
		State& s_copy= *s0->clone();
		list<State*> S;
		S.push_back(&s_copy);

		g->next(S);

		while(!S.empty())
			gr->next(S);

		val+=s_copy.get_value();
	}
	return val/100.0;
}

void generate_data(string file, double min_fr, int max_time, double alpha, double beta, double gamma, double p,
		double r=0.0, double min_vol=0.0, int l=6, int w=1, int h=2, int histo_ranges=20, int max_size=120){

	Space::bottom_up=false;
	int inst=0;

	//cout << file<<endl;
  //cout << "cargando la instancia..." << endl;

    Block::FSB=false;
    clpState* s0 = new_state(file,inst, min_fr, 1000);

  //  cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs_gr = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, 1.0, 0.0, r);

	VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
		alpha, beta, gamma, p, 1.0, 0.0, 0.0);
	VCS_Function* vloss = new VCS_Function(s0->nb_left_boxes, *s0->cont,
		0.0, beta, gamma, 0.0, 1.0, 0.0, 0.0);
	VCS_Function* cs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
		alpha, 0.0, gamma, p, 1.0, 0.0, 0.0);

      Greedy *gr = new Greedy (vcs_gr);
	  Greedy *gr_vcs = new Greedy (vcs);
	  Greedy *gr_vloss = new Greedy (vloss);
	  Greedy *gr_cs = new Greedy (cs);

	  list<State*> S;
	  S.push_back(s0);
	  while(!S.empty() && s0->get_value()<min_vol)
		  gr->next(S);

	  compactState c(*s0);


	  double vcs_eval=greedy_eval(gr_vcs, gr, s0);
	  double vloss_eval=greedy_eval(gr_vloss, gr, s0);
	  double cs_eval=greedy_eval(gr_cs, gr, s0);
	  double mean = (vcs_eval + vloss_eval + cs_eval)/3.0;


      double m = max(vcs_eval-mean,vloss_eval-mean);
      m = max(vloss_eval-mean,cs_eval-mean);
      if (m>0.0001){
    	  cout << c ;
    	  cout << vcs_eval << "," << vloss_eval << "," << cs_eval << endl;
      }

		//cout << "bsg solving" << endl;
	  /*BSG *bsg= new BSG(vcs,*gr2, 4);
	  SearchStrategy *de= new DoubleEffort(*bsg);
	  State& s_copy= *s0->clone();
	  double eval = de->run(s_copy, max_time, begin_time) ;
	  cout << eval << endl;*/

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

	args::ValueFlag<int> _sl(parser, "int", "(compactState) Slides in length", {"sl"});
	args::ValueFlag<int> _sw(parser, "int", "(compactState) Slides in width", {"sw"});
	args::ValueFlag<int> _sh(parser, "int", "(compactState) Slides in height", {"sh"});
	args::ValueFlag<int> _histo_ranges(parser, "int", "(compactState) Number of ranges for the histograms", {"hist_ranges"});
	args::ValueFlag<double> _min_vol(parser, "double", "(greedy) Min vol criteria", {"min_vol"});
	args::ValueFlag<double> _r(parser, "double", "(Greedy) Random weight", {'r'});

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

  int sl=8, sw=3, sh=3, histo_ranges=20, max_size=_max_l;
	if(_sl) sl=_sl.Get();
	if(_sw) sw=_sh.Get();
	if(_sh) sh=_sh.Get();
	if(_histo_ranges) histo_ranges=_histo_ranges.Get();



	double _bsl=2.0;
	if(bsl) _bsl=bsl.Get();

	int _s=1;
	if(s) _s=s.Get();
	srand(_s);

	double _min_fr=(_N>25)? 0.98:1.0;
	int _maxtime=2;
	double _alpha=4.0, _beta=1.0, _gamma=0.2, _p=0.04, r=0.0, min_vol=1.0;
	if(maxtime) _maxtime=maxtime.Get();
	if(alpha) _alpha=alpha.Get();
	if(beta) _beta=beta.Get();
	if(gamma) _gamma=gamma.Get();
	if(p) _p=p.Get();
	if(_r) r=_r.Get();
	if(min_fr) _min_fr=min_fr.Get();
	if(_min_vol) min_vol=_min_vol.Get();


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


		generate_data(file, _min_fr, _maxtime, _alpha, _beta, _gamma, _p, r, min_vol, sl, sw, sh, histo_ranges, max_size);
		char rm[128];
		strcpy(rm,"rm ");
		strcat(rm,file.c_str());
		int i=system(rm);
	}



}
