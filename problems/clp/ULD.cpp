/*
 * ULD.cpp
 */

#include <fstream>
#include <sstream>
#include <iostream>
#include "ULD.h"

using namespace std;

namespace clp {

vector<ULD> ULD::read(const string& file){
	ifstream in(file.c_str());
	if(!in){
		cerr << "No se pudo abrir el archivo " << file << endl;
		exit(1);
	}

	//cada linea: L;W;H;peso maximo;costo;desviaciones CoG (x,y,z);4 recortes (a,b,c);volumen;tipo
	//el recorte (a,b,c) es la recta a*z + b*x = c en el plano x-z (Paquay et al. 2016)
	vector<ULD> ulds;
	string line;
	while(getline(in, line)){
		vector<double> f;
		stringstream ss(line);
		string tok;
		while(getline(ss, tok, ';')) if(!tok.empty()) f.push_back(atof(tok.c_str()));
		if(f.size()<22) continue;

		ULD u;
		u.L=(long) f[0]; u.W=(long) f[1]; u.H=(long) f[2];
		u.max_weight=f[3]; u.cost=f[4];
		for(int d=0; d<3; d++) u.dev[d]=(long) f[5+d];
		for(int k=0; k<4; k++){
			long a=(long) f[8+3*k], b=(long) f[9+3*k], c=(long) f[10+3*k];
			if(a==0 && b==0 && c==0) continue;
			//se orienta el semiplano para que contenga al centro del ULD
			Cut cut={b, a, c};
			if(cut.nx*u.L + cut.nz*u.H > 2*cut.c){ cut.nx=-cut.nx; cut.nz=-cut.nz; cut.c=-cut.c; }
			u.cuts.push_back(cut);
		}
		u.volume=f[20];
		u.type=(int) f[21];
		ulds.push_back(u);
	}
	return ulds;
}

} /* namespace clp */
