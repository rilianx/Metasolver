/*
 * Container.h
 *
 * Representacion del contenedor: bloques ubicados y espacios libres maximales.
 * Ambos se guardan en vectores (memoria contigua), siguiendo la propuesta
 * de L. Guesser (cap. 4): las busquedas de intersecciones/contenciones se
 * hacen recorriendo el vector, lo que es mas rapido que estructuras basadas
 * en nodos (arboles de Bullet, std::set, std::list) para la cantidad de
 * espacios que maneja el algoritmo.
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include <vector>
#include "Space.h"
#include "Block.h"

namespace clp {

class Container {
public:
	Container(long L, long W, long H) : L(L), W(W), H(H), occupied_volume(0) {
		spaces.push_back(Space(AABB(0,0,0,L,W,H), L, W, H));
	}

	long getL() const { return L; }
	long getW() const { return W; }
	long getH() const { return H; }
	long getVolume() const { return L*W*H; }
	long getOccupiedVolume() const { return occupied_volume; }

	/** indice del espacio con mayor prioridad (ver Space::operator<), -1 si no hay */
	int top_space() const {
		int best=-1;
		for(int i=0; i<(int)spaces.size(); i++)
			if(best<0 || spaces[i] < spaces[best]) best=i;
		return best;
	}

	void remove_space(int i){
		spaces[i]=spaces.back();
		spaces.pop_back();
	}

	/**
	 * Ubica el bloque b en la posicion (x,y,z) y actualiza los espacios libres.
	 * Los espacios resultantes con alguna dimension menor a las de min_dim se descartan.
	 */
	void insert(const Block& b, long x, long y, long z, long min_l, long min_w, long min_h);

	const long L, W, H;
	long occupied_volume;
	std::vector<Space> spaces;
	std::vector<PlacedBlock> blocks;
};

} /* namespace clp */

#endif /* CONTAINER_H_ */
