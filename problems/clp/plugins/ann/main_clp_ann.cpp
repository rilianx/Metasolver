/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
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

bool global::TRACE = false;

using namespace std;

// para ejecutar (menos de 30 tipos de caja): BSG_CLP problems/clp/benchs/BR/BR7.txt 1 1.0 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0
// para ejecutar (mas de 30 tipos de caja): BSG_CLP problems/clp/benchs/BR/BR8.txt 1 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0

int n_tipos_cajas(string file);
void whiteBackground( vector< vector< int > >& , int ) ;
void printBlock(vector< vector< int > >& , vector<int> , vector<int> ,int );
void printPgm(string ,vector< vector< int > > ,int  );
string outputPath(string);
void extract_boxes(AABB aabb, list< AABB >& uni_blocks);
void addHeader(string, int , int,ofstream&);

/**
 * comparacion para ordenar de menor a mayor altura
 */
bool compare_aabb(AABB& aabb1, AABB& aabb2){
	return aabb1.getZmin() < aabb2.getZmin();
}

int main(int argc, char** argv){

	Space::bottom_up=true;

	string file=argv[1];
	int inst=atoi(argv[2]);
	double min_fr=atof(argv[3]); //<-- 0.98 o 1.0

	int max_time=atoi(argv[4]); //500
    double alpha=atof(argv[5]); //4.0
    double beta=atof(argv[6]); //1.0
    double gamma=atof(argv[7]); //0.2
    double p=atof(argv[8]); //0.04
    double delta=atof(argv[9]); //1.0
	double f=atof(argv[10]); //0.0
    double r=atof(argv[11]); //0.0
    bool fsb=(atoi(argv[12])==1); //0
    bool kdtree=atoi(argv[13]); //0

	srand(1);



	//SpaceSet::random_spaces=true;
    //global::TRACE=true;
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

	//for(int i=0;i<10000; i++)
	//	exp->best_action(*s0);

	cout << "greedy" << endl;
    SearchStrategy *gr = new Greedy (vcs);

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
	cout << "eval"<<1-eval << endl;


	string path=outputPath(file);

//hasta ahora el path de output seria ej: "problems/clp/train_samples/10/"

		cout<<"path:"<<path<<endl;


		stringstream ss;
		ss << inst;
		string indiceInstancia = ss.str();

		cout <<"indice instancia"<<indiceInstancia<<endl;

		path=path+"trainSample";

		cout<<"path:"<<path<<endl;

	list<const Action*>& actions= dynamic_cast<const clpState*>(de->get_best_state())->get_path();

	clpState* s00 = dynamic_cast<clpState*> (s0->clone());

	const AABB* aux= &s00->cont->blocks->top();
	int width=587,length=233, maxval=220;

	ofstream myfile;
	ofstream myfile1;

	string path2;
	path2=path+"Box"+indiceInstancia+".txt";
	path=path+"Block"+indiceInstancia+".txt";
	myfile.open(path);
	myfile1.open(path2);

	myfile << eval <<endl;

	myfile1 << eval <<endl;

	int n_tipos = n_tipos_cajas(file);

	addHeader( file,  inst, n_tipos, myfile);
	addHeader( file,  inst, n_tipos, myfile1);

	cout<<"path:"<<path<<endl;


	for(auto action:actions){

		const clpAction* clp_action = dynamic_cast<const clpAction*> (action);

		const AABB* aux= &s00->cont->blocks->top();

		s00->transition(*clp_action);


		cout << "block :" << clp_action->block << endl;

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

	}
	myfile.close();
	myfile1.close();

	//s00->cont->MatLab_print();


}

void addHeader(string input_path, int inst,int tipos, ofstream& output){
	string line;
	  ifstream myfile (input_path);

	  int limsup, liminf;

	  liminf=1+(inst)*(tipos+3);
	  limsup=liminf+tipos+3;

	  cout << "tipos:" <<tipos<<endl;
	  cout <<"lim inf:"<<liminf<<endl;
	  cout <<"lim sup:"<<limsup<<endl;
	  if (myfile.is_open())
	  {
		    for (int lineno = 0; getline (myfile,line) && lineno < limsup; lineno++)
		      if (lineno >= liminf)
		    	  output << line << endl;

	  }

	  else cout << "Unable to open file";

}

int n_tipos_cajas(string file){
	if(file.compare("problems/clp/benchs/ann_a/data01.txt")==0) return 1;//direccion de salida particular
	if(file.compare("problems/clp/benchs/ann_a/data03.txt")==0) return 3;
	if(file.compare("problems/clp/benchs/ann_a/data05.txt")==0) return 5;
	if(file.compare("problems/clp/benchs/ann_a/data08.txt")==0) return 8;
	if(file.compare("problems/clp/benchs/ann_a/data10.txt")==0) return 10;
	if(file.compare("problems/clp/benchs/ann_a/data12.txt")==0) return 12;
	if(file.compare("problems/clp/benchs/ann_a/data15.txt")==0) return 15;
	if(file.compare("problems/clp/benchs/ann_a/data20.txt")==0) return 20;
	if(file.compare("problems/clp/benchs/ann_a/data30.txt")==0) return 30;
	if(file.compare("problems/clp/benchs/ann_a/data40.txt")==0) return 40;
	if(file.compare("problems/clp/benchs/ann_a/data50.txt")==0) return 50;
	if(file.compare("problems/clp/benchs/ann_a/data60.txt")==0) return 60;
	if(file.compare("problems/clp/benchs/ann_a/data70.txt")==0) return 70;
	if(file.compare("problems/clp/benchs/ann_a/data80.txt")==0) return 80;
	if(file.compare("problems/clp/benchs/ann_a/data90.txt")==0) return 90;
	if(file.compare("problems/clp/benchs/ann_a/data100.txt")==0) return 100;

	return 0;

}
string outputPath(string file){
	string path1="problems/clp/benchs/ann_a/"; //directorio de entrada;
	 string path="problems/clp/train_samples/"; //directorio de salida;
		if(file.compare(path1+"data01.txt")==0)path=path+"1/";//direccion de salida particular
		if(file.compare(path1+"data03.txt")==0)path=path+"3/";
		if(file.compare(path1+"data05.txt")==0)path=path+"5/";
		if(file.compare(path1+"data08.txt")==0)path=path+"8/";
		if(file.compare(path1+"data10.txt")==0)path=path+"10/";
		if(file.compare(path1+"data12.txt")==0)path=path+"12/";
		if(file.compare(path1+"data15.txt")==0)path=path+"15/";
		if(file.compare(path1+"data20.txt")==0)path=path+"20/";
		if(file.compare(path1+"data30.txt")==0)path=path+"30/";
		if(file.compare(path1+"data40.txt")==0)path=path+"40/";
		if(file.compare(path1+"data50.txt")==0)path=path+"50/";
		if(file.compare(path1+"data60.txt")==0)path=path+"60/";
		if(file.compare(path1+"data70.txt")==0)path=path+"70/";
		if(file.compare(path1+"data80.txt")==0)path=path+"80/";
		if(file.compare(path1+"data90.txt")==0)path=path+"90/";
		if(file.compare(path1+"data100.txt")==0)path=path+"100/";

	return path;
}

void extract_boxes(AABB aabb, list< AABB >& uni_blocks){

	const Block* block=aabb.getBlock();
	//cout << block << endl;

	Vector3 pos = aabb.getMins();

	if(block->n_boxes==1){
		uni_blocks.push_back(aabb);
		return;
	}

	const AABB* aux= &block->blocks->top();
	while(true){

		AABB aabb2 (pos+aux->getMins(), aux->getBlock());
		extract_boxes(aabb2, uni_blocks);

		if(block->blocks->has_next())
			aux=&block->blocks->next();

		else break;
	}

}


void whiteBackground( vector< vector< int > >& imageMatrix, int maxval) {
	int width=imageMatrix.size();//filas
	int length=imageMatrix[0].size();//columnas
	for(int w=0;w<width;w++){
		for(int l=0;l<length;l++){
			imageMatrix[w][l]=maxval;//dejo la imagen en blanco
		}
	}

}

void printBlock(vector< vector< int > >& imageMatrix, vector<int> tamano, vector<int> ubicacion,int maxVal){
	int width=imageMatrix.size();//filas
	int length=imageMatrix[0].size();//columnas
	for(int w=0;w<width;w++){
		for(int l=0;l<length;l++){
			if(true){
				imageMatrix[w][l]=maxVal;//dejo la imagen en blanco
			}
		}
	}
}

void printPgm(string filename,vector< vector< int > > imageMatrix,int maxVal ){
	ofstream img (filename+".pgm");
	int width=imageMatrix.size();//filas
	int length=imageMatrix[0].size();//columnas

	img <<  "P2" << endl;
	img << width << " " << length<< endl ;
	img << "255" << endl;
	img << maxVal<< endl;
		for(int x = 0; x < width; x++){
			for(int y=0;y < length;y++){

				img << imageMatrix[x][y] <<" ";


		}
		img << endl;
	}
		img.close();
}








