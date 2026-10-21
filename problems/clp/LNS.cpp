/*
 * LNS.cpp
 */

#include <cmath>
#include <algorithm>
#include <iostream>
#include "LNS.h"
#include "BSG.h"

using namespace std;

namespace clp {

clpState* clpLNS::rebuild(const vector<PlacedBlock>& blocks) const{
	clpState* s = static_cast<clpState*>(s0.clone());
	long L=s0.cont.getL(), W=s0.cont.getW(), H=s0.cont.getH();
	for(const PlacedBlock& pb : blocks){
		//el espacio tiene exactamente el tamano del bloque, por lo que el bloque
		//queda en la misma posicion
		clpAction action(*pb.block, Space(pb, L, W, H));
		s->transition(action);
	}
	return s;
}

vector<PlacedBlock> clpLNS::destroy(const clpState& s, int k){
	double dims[3]={(double) s0.cont.getL(), (double) s0.cont.getW(), (double) s0.cont.getH()};

	//centro desplazado aleatoriamente (a lo mas un 10% de cada dimension)
	uniform_real_distribution<double> u(-0.1, 0.1);
	double c[3];
	for(int i=0; i<3; i++) c[i] = dims[i]*(0.5 + u(rng));

	const vector<PlacedBlock>& B = s.cont.blocks;
	vector< pair<double,int> > dist;
	for(size_t i=0; i<B.size(); i++){
		double p[3]={(B[i].xmin+B[i].xmax)/2.0, (B[i].ymin+B[i].ymax)/2.0, (B[i].zmin+B[i].zmax)/2.0};
		double d = fabs(p[0]-c[0])/dims[0];
		if(shape==BOX)
			for(int j=1; j<3; j++) d = max(d, fabs(p[j]-c[j])/dims[j]);
		dist.push_back(make_pair(d, i));
	}
	sort(dist.begin(), dist.end());

	vector<bool> removed(B.size(), false);
	for(int i=0; i<k && i<(int)dist.size(); i++) removed[dist[i].second]=true;

	//se mantiene el orden original de los bloques
	vector<PlacedBlock> keep;
	for(size_t i=0; i<B.size(); i++)
		if(!removed[i]) keep.push_back(B[i]);
	return keep;
}

clpState* clpLNS::run(const clpState& s, double tl, clock_t bt){
	auto time = [bt](){ return double(clock()-bt)/double(CLOCKS_PER_SEC); };

	clpState* best = static_cast<clpState*>(s.clone());
	double kf=k0;

	while(time() < tl){
		//destruccion: se eliminan los k bloques mas cercanos al centro
		int n = best->cont.blocks.size();
		int k = max(2, (int) lround(kf*n));
		if(k>=n) break;
		vector<PlacedBlock> keep = destroy(*best, k);

		//reparacion: BSG desde la solucion parcial (el BSG libera el estado)
		clpState* partial = rebuild(keep);
		BSG bsg(evl, greedy, beams, 0.0, 0, false);
		bsg.run(*partial, tl, bt);
		iterations++;

		const State* r = bsg.get_best_state();
		if(!r || r->get_value() < best->get_value()) { kf=min(kf*growth, kmax); continue; }

		//se aceptan soluciones iguales (diversificacion), la cantidad de bloques
		//eliminados solo vuelve a su valor inicial si hay mejora
		if(r->get_value() > best->get_value()){
			kf=k0;
			cout << "[LNS] new best_solution_found (" << time() << "): " << r->get_value()
					<< " (iteration " << iterations << ")" << endl;
		}else
			kf=min(kf*growth, kmax);

		delete best;
		best = static_cast<clpState*>(r->clone());
	}

	return best;
}

} /* namespace clp */
