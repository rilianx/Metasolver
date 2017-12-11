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

void whiteBackground( vector< vector< int > >& , int ) ;
void printBlock(vector< vector< int > >& , vector<int> , vector<int> ,int );
void printPgm(string ,vector< vector< int > > ,int  );


int main(int argc, char** argv){

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
	cout << eval << endl;
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


//hasta ahora el path de output seria ej: "problems/clp/train_samples/10/"




		cout<<"path:"<<path<<endl;


		stringstream ss;
		ss << inst;
		string indiceInstancia = ss.str();

		cout <<"indice instancia"<<indiceInstancia<<endl;

		path=path+"trainSample";

		cout<<"path:"<<path<<endl;

	list<const Action*>& actions= dynamic_cast<const clpState*>(de->get_best_state())->get_path();
    actions.sort(clpState::height_sort);


	clpState* s00 = dynamic_cast<clpState*> (s0->clone());



	const AABB* aux= &s00->cont->blocks->top();
	int width=587,length=233, maxval=220;

	//int image[width][length];

	 //vector<vector<int> > imageMatrix(width, vector<int>(length, 0));

	//whiteBackground(imageMatrix, maxval);
	ofstream myfile;
	path=path+indiceInstancia+".txt";
	myfile.open(path);


	cout<<"path:"<<path<<endl;




	//ofstream img ("picture.pgm");
	//recorro cada accion
	string sampleName=path;
	int i=0;
	for(auto action:actions){

		const clpAction* clp_action = dynamic_cast<const clpAction*> (action);
		//ofstream img ("picture"+i+".pgm");
		//recorro los bloques
		const AABB* aux= &s00->cont->blocks->top();
		cout<<"recorro los bloques"<<endl;
	    while(true){
			/*//...
	    	//aqui deberia formar
			if(!(s00->is_root())) {
			cout<<"bloque:"<<endl;
	    	cout<<"xmax:"<<aux->getXmax()<<endl;
	    	cout<<"ymax:"<<aux->getYmax()<<endl;
	    	cout<<"zmax:"<<aux->getZmax()<<endl;
	    	cout<<"xmin:"<<aux->getXmin()<<endl;
	    	cout<<"ymin:"<<aux->getYmin()<<endl;
	    	cout<<"zmin:"<<aux->getZmin()<<endl;

			}else{
				cout<<"no hay bloques"<<endl;
				//string filename=
				//printPgm();
			}*/
	    	if(s00->cont->blocks->has_next())
			  aux=&s00->cont->blocks->next();

	    	else break;
		}
	    //pongo un bloque
		s00->transition(*clp_action);
		//s00->nb_left_boxes;
		//cout<<"pongo un bloque"<<endl;
		//imprimo el bloque puesto y su posicion

		cout << "block :" << clp_action->block << endl;
		myfile << clp_action->block << endl;
		cout << "location :" << clp_action->space.get_location(clp_action->block) << endl;
		myfile << clp_action->space.get_location(clp_action->block) << endl;

		i++;
	}
	myfile.close();

/*
	const AABB* b = &dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->top();
	while(dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->has_next()){
		cout << *b << ":" << b->getVolume() << "(" << b->getOccupiedVolume() << ")" << endl;
		b = &dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->next();
	}
*/

	//s00->cont->MatLab_print();


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








