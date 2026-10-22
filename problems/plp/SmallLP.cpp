/*
 * SmallLP.cpp
 */

#include <algorithm>
#include "SmallLP.h"

using namespace std;

namespace plp {

namespace {

/** resuelve G y = r (k <= 3) por eliminacion gaussiana con pivoteo parcial */
bool solve(int k, double G[3][3], double r[3], double y[3]) {
	for(int i=0; i<k; i++){
		int p=i;
		for(int j=i+1; j<k; j++) if(fabs(G[j][i]) > fabs(G[p][i])) p=j;
		if(fabs(G[p][i]) < 1e-300) return false;
		for(int j=0; j<k; j++) swap(G[i][j], G[p][j]);
		swap(r[i], r[p]);
		for(int j=i+1; j<k; j++){
			double f = G[j][i]/G[i][i];
			for(int l=i; l<k; l++) G[j][l] -= f*G[i][l];
			r[j] -= f*r[i];
		}
	}
	for(int i=k-1; i>=0; i--){
		double s=r[i];
		for(int j=i+1; j<k; j++) s -= G[i][j]*y[j];
		y[i] = s/G[i][i];
	}
	return true;
}

}

Vec3 SmallLP::minimize(const Vec3& c, const Vec3& x0, bool* optimal, int maxit) const {
	Vec3 x = x0;
	vector<int> W;   //restricciones activas (linealmente independientes)
	vector<bool> inW(A.size(), false);
	double cn = c.norm();
	if(optimal) *optimal=false;

	for(int it=0; it<maxit; it++){
		//base ortonormal de las filas activas
		Vec3 Q[3];
		for(size_t k=0; k<W.size(); k++){
			Vec3 q = A[W[k]];
			for(size_t l=0; l<k; l++) q = q - Q[l]*dot(Q[l], q);
			Q[k] = q / q.norm();
		}

		//direccion: -c proyectado en el espacio nulo de las filas activas
		Vec3 p = -c;
		for(size_t k=0; k<W.size(); k++) p = p - Q[k]*dot(Q[k], p);

		if(p.norm() > 1e-12*cn){
			double alpha = numeric_limits<double>::max();
			int block = -1;
			for(size_t i=0; i<A.size(); i++){
				if(inW[i]) continue;
				double ap = dot(A[i], p);
				if(ap <= 1e-12*A[i].norm()*p.norm()) continue;
				double s = max(0.0, (B[i] - dot(A[i], x)) / ap);
				if(s < alpha) { alpha=s; block=i; }
			}
			if(block<0) return x;  //no acotado (no ocurre si hay cotas para cada variable)
			x = x + p*alpha;
			W.push_back(block);
			inW[block]=true;
			continue;
		}

		//multiplicadores: c + sum lambda_k a_k = 0
		int k = W.size();
		if(k==0) { if(optimal) *optimal=true; return x; }
		double G[3][3], r[3], lambda[3];
		for(int i=0; i<k; i++){
			for(int j=0; j<k; j++) G[i][j] = dot(A[W[i]], A[W[j]]);
			r[i] = -dot(A[W[i]], c);
		}
		if(!solve(k, G, r, lambda)) return x;

		int out=-1;
		double lmin=-1e-12*cn;
		for(int i=0; i<k; i++) if(lambda[i] < lmin) { lmin=lambda[i]; out=i; }
		if(out<0) { if(optimal) *optimal=true; return x; }
		inW[W[out]]=false;
		W.erase(W.begin()+out);
	}
	return x;
}

} /* namespace plp */
