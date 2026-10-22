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
		regions.push_back(AABB(0,0,0,L,W,H));
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

	/** indice del espacio con mayor prioridad de la region r, -1 si no hay */
	int top_space(int r) const {
		int best=-1;
		for(int i=0; i<(int)spaces.size(); i++)
			if(spaces[i].region==r && (best<0 || spaces[i] < spaces[best])) best=i;
		return best;
	}

	/** region (contenedor) que contiene a la caja b */
	int region_of(const AABB& b) const {
		for(int r=0; r<(int)regions.size(); r++) if(regions[r].contains(b)) return r;
		return -1;
	}

	void remove_space(int i){
		spaces[i]=spaces.back();
		spaces.pop_back();
	}

	/**
	 * El espacio i deja de considerarse hasta la siguiente insercion (no hay bloques que se
	 * puedan apoyar en su base, pero un bloque ubicado despues podria darle apoyo)
	 */
	void make_dormant(int i){
		dormant.push_back(spaces[i]);
		remove_space(i);
	}

	/**
	 * Espacios iniciales de un ULD con un borde de ancho e en el piso, sobre el que no se
	 * puede cargar bajo la altura delta. Con subestructura, el piso se eleva a la altura delta
	 */
	void set_floor(long e, long delta, bool substructure){
		spaces.clear();
		add_floor(0, e, delta, substructure);
	}

	/**
	 * Varios contenedores (regiones disjuntas) que se cargan a la vez, sin espacios iniciales
	 * (ver add_floor)
	 */
	void set_regions(const std::vector<AABB>& r){
		regions=r;
		spaces.clear();
	}

	/** espacios iniciales de la region r (ver set_floor) */
	void add_floor(int r, long e, long delta, bool substructure){
		const AABB& b=regions[r];
		if(substructure)
			spaces.push_back(Space(AABB(b.xmin,b.ymin,b.zmin+delta,b.xmax,b.ymax,b.zmax), b, r));
		else{
			spaces.push_back(Space(AABB(b.xmin+e,b.ymin+e,b.zmin,b.xmax-e,b.ymax-e,b.zmax), b, r));
			if(e>0) spaces.push_back(Space(AABB(b.xmin,b.ymin,b.zmin+delta,b.xmax,b.ymax,b.zmax), b, r));
		}
	}

	/**
	 * Ubica el bloque b en la posicion (x,y,z) y actualiza los espacios libres.
	 * Los espacios resultantes con alguna dimension menor a las de min_dim se descartan.
	 */
	void insert(const Block& b, long x, long y, long z, long min_l, long min_w, long min_h);

	const long L, W, H;
	long occupied_volume;
	//contenedores (una region, o varios cuando se cargan a la vez)
	std::vector<AABB> regions;
	std::vector<Space> spaces;
	//espacios sin apoyo (se reactivan al insertar un bloque)
	std::vector<Space> dormant;
	std::vector<PlacedBlock> blocks;
};

} /* namespace clp */

#endif /* CONTAINER_H_ */
