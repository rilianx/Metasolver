/*
 * Mesh.h
 *
 * Mallas poligonales (lectura de archivos OBJ y STL)
 */

#ifndef PLP_MESH_H_
#define PLP_MESH_H_

#include <string>
#include <vector>
#include "Geometry.h"

namespace plp {

struct Mesh {
	std::vector<Vec3> vertices;

	//caras (poligonos) como indices de vertices
	std::vector< std::vector<int> > faces;

	//grupos de caras ('o' o 'g' en OBJ): el grupo k contiene las caras [groups[k], groups[k+1])
	std::vector<int> groups;

	/** OBJ o STL (ASCII o binario) segun la extension */
	static Mesh load(const std::string& file);

	static Mesh load_obj(const std::string& file);
	static Mesh load_stl(const std::string& file);

	int n_groups() const { return groups.size(); }

	/**
	 * Separa las caras en cascarones cerrados consecutivos (cada arista usada por
	 * exactamente dos caras). Asi vienen las uniones de convexos de Stoyan et al. (2004).
	 * Retorna los rangos [begin,end) de cada cascaron o vacio si las caras no se pueden separar asi.
	 */
	std::vector< std::pair<int,int> > closed_shells(int begin, int end) const;

	/** true si cada arista de las caras [begin,end) es usada por exactamente dos caras */
	bool is_closed(int begin, int end) const;

	/** volumen encerrado por las caras [begin,end) (cascaron cerrado y orientado) */
	double volume(int begin, int end) const;
};

} /* namespace plp */

#endif /* PLP_MESH_H_ */
