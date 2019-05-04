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
#include "BlockSet.h"
#include "SpaceSet.h"


using namespace std;

/**
 * comparacion para ordenar de menor a mayor altura
 */
bool compare_aabb(AABB& aabb1, AABB& aabb2){
	return aabb1.getZmin() < aabb2.getZmin();
}

void generaInstancia(int l, int w, int h, int n, int a1, int a2, int a3, int b1,int b2, int b3, double L, int s, const char* file){

	ofstream myfile;
	myfile.open(file);

	float t_c= l*h*w;	//-T_c:Cargo target volumen
	int low_bound[3]={a1,a2,a3};	// limites inferiores  de las dimensiones de las cajas
	int upper_bound[3]={b1,b2,b3};	//Limites superiores de las dimensiones de las cajas


	float dimensiones_caja[n][3];//caja,alto,ancho, largo
	int cantidad_caja_tipo[n];
	float volumen_caja_tipo[n];
	int orientacion_permitida[n][3];//si esta permitida esa dimension de forma vertical
	float cargo_volumen=0;//volumen de la carga
	//y descartar primeros 10 numeros generados por este
	int k=10;
	while(k-- >0)
	rand();
	myfile<<1<<endl;
	myfile<<1<<" "<<s<<endl;
	myfile<<l<<" "<<w<<" "<<h<<endl;
	myfile<<n<<endl;
		for(int i=0;i<n;i++){
			int r_j[3]={rand(),rand(),rand()};
			for(int j=0;j<3;j++){//por cada dimension de la caja
				dimensiones_caja[i][j]=low_bound[j]+(r_j[j]%(upper_bound[j]-low_bound[j]+1));
				//inicializo la cantidad de cajas de cada tipo en uno
			}
			cantidad_caja_tipo[i]=1;
			volumen_caja_tipo[i]=dimensiones_caja[i][0]*dimensiones_caja[i][1]*dimensiones_caja[i][2];

			int min_dim=99999;
			for(int j=0;j<3;j++){
				if(dimensiones_caja[i][j]<min_dim)min_dim=dimensiones_caja[i][j];
			}
			for(int j=0;j<3;j++){
				if((double)dimensiones_caja[i][j]/(double)min_dim<L)
					{
					orientacion_permitida[i][j]=1;
					}else{
					orientacion_permitida[i][j]=0;
					}
			}
		}

		//calcular volumen ocupado

		int v_k=0;
		bool flag=true;
		 while(flag){
			cargo_volumen=0;
			for(int i=0;i<n;i++){
				cargo_volumen+=cantidad_caja_tipo[i]*volumen_caja_tipo[i];
			}
			int aux=rand()%n;
			v_k= volumen_caja_tipo[aux];

			if(t_c >cargo_volumen+v_k){
				cantidad_caja_tipo[aux]++;
			}else{

				break;

			}
		 }

		 for(int i=0;i<n;i++){
			 myfile<<i+1;
			 for(int j=0;j<3;j++){
					myfile <<" "<< dimensiones_caja[i][j]<<" "<< orientacion_permitida[i][j]<<" ";
			 }
			 myfile<<cantidad_caja_tipo[i]<<endl;

		 }

	myfile.close();
}

void addHeader(string input_path, int tipos, ofstream& output){

	  int inst=0;
	  string line;
	  ifstream myfile (input_path);

	  int limsup, liminf;

	  liminf=1+(inst)*(tipos+3);
	  limsup=liminf+tipos+3;


	  if (myfile.is_open()){
		    for (int lineno = 0; getline (myfile,line) ; lineno++)
		    	output << line << endl;
	  }

	  else cout << "Unable to open file";

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
