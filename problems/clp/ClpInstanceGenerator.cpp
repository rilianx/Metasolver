//============================================================================
// Name        : ClpInstanceGenerator.cpp
// Author      : ArielMarchant
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;
//crea una instancia de problema container loading problem

void generaInstancia(int,float,float,float,int,int,int,int,int,int,int,float,int, ostream&);

int main() {
	//input parameters:
	//int p=1;
	ofstream myfile;
	myfile.open("problems/clp/benchs/ann_a/data01.txt");
	int semilla;
	int n_casos=100;
	int n_tipos_cajas=1;
    myfile<<n_casos<<endl;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data03.txt");
    n_tipos_cajas=3;
    myfile<<n_casos<<endl;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data05.txt");
	myfile<<n_casos<<endl;
	n_tipos_cajas=5;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data08.txt");
	myfile<<n_casos<<endl;
	n_tipos_cajas=8;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data10.txt");
	myfile<<n_casos<<endl;
	n_tipos_cajas=10;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data12.txt");
	myfile<<n_casos<<endl;
	n_tipos_cajas=12;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data15.txt");
	myfile<<n_casos<<endl;
	n_tipos_cajas=15;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data20.txt");
	myfile<<n_casos<<endl;
	n_tipos_cajas=20;
	for(int i=1;i<=n_casos;i++){
		semilla=101+100*(i-1);
		generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
	}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data30.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=30;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data40.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=40;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data50.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=50;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data60.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=60;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data70.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=70;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data80.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=80;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data90.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=90;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	myfile.open("problems/clp/benchs/ann_a/data100.txt");
		myfile<<n_casos<<endl;
		n_tipos_cajas=100;
		for(int i=1;i<=n_casos;i++){
			semilla=101+100*(i-1);
			generaInstancia(i,587,233,220,n_tipos_cajas,30,25,20,120,100,80,2,semilla,myfile);
		}
	myfile.close();
	return 0;
}

void generaInstancia(int caso,float l,float w,float h,int n, int a1,int a2, int a3, int b1,int b2, int b3, float L,int s, ostream& myfile){
	float t_c= l*h*w;	//-T_c:Cargo target volumen
	int low_bound[3]={a1,a2,a3};	// limites inferiores  de las dimensiones de las cajas
	int upper_bound[3]={b1,b2,b3};	//Limites superiores de las dimensiones de las cajas
	srand(s);
	float dimensiones_caja[n][3];//caja,alto,ancho, largo
	int cantidad_caja_tipo[n];
	float volumen_caja_tipo[n];
	int orientacion_permitida[n][3];//si esta permitida esa dimension de forma vertical
	float cargo_volumen=0;//volumen de la carga
	//y descartar primeros 10 numeros generados por este
	int k=10;
	while(k-- >0)
	rand();
	myfile<<caso<<" "<<s<<endl;
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
			float min_dim=99999;
			for(int j=0;j<3;j++){
				if(dimensiones_caja[i][j]<min_dim)min_dim=dimensiones_caja[i][j];
			}
			for(int j=0;j<3;j++){
				if(dimensiones_caja[i][j]/min_dim<L)
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

}

