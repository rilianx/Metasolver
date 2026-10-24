/*
 * Instance.h
 *
 * Instancias del problema de carga de poliedros (3D irregular strip packing):
 * un contenedor de base W x D fija y altura abierta, y tipos de pieza con su demanda.
 * Formato JSON de ESICUP (https://github.com/ESICUP/datasets/tree/main/3d_irregular).
 */

#ifndef PLP_INSTANCE_H_
#define PLP_INSTANCE_H_

#include <string>
#include <vector>
#include "ConvexPolytope.h"

namespace plp {

/**
 * Pieza como union de politopos convexos.
 * Las coordenadas se trasladan para que la caja envolvente parta en el origen
 * (convencion "AlignToMinimum" de las soluciones de ESICUP).
 */
class Piece {
public:

	/**
	 * Lee una malla OBJ o STL. Las partes son los grupos del OBJ ('o'/'g') o, si no
	 * hay grupos, los cascarones cerrados consecutivos de la malla.
	 * Si parts_file no es vacio, las partes se leen de ese archivo (descomposicion convexa).
	 */
	static Piece load(const std::string& file, const std::string& parts_file="");

	std::string name;

	//archivo de la descomposicion convexa (vacio si las partes vienen de la malla)
	std::string parts_file;

	//partes convexas (si convex==false, son las envolventes convexas de las partes)
	std::vector<ConvexPolytope> parts;

	//todas las partes son convexas: la pieza se representa exactamente
	bool convex_parts;

	//la pieza completa es convexa
	bool convex;

	double volume;

	Box3 bbox;

	//esquina minima de la caja envolvente en las coordenadas del archivo
	Vec3 offset;

	std::vector<std::string> warnings;
};

struct ItemType {
	std::string path;
	int piece;
	int demand;
};

class Instance {
public:

	/**
	 * Lee la instancia. Las rutas de las piezas son relativas a la carpeta raiz del
	 * conjunto de datos; si root es vacio se busca subiendo desde la carpeta del archivo.
	 * Si existe root/decompositions/<ruta de la pieza con extension .obj>, las partes
	 * convexas de la pieza se leen de ese archivo.
	 */
	/** dec: carpeta (bajo root) con las descomposiciones convexas de las piezas */
	static Instance load(const std::string& file, const std::string& root="", const std::string& dec="decompositions");

	int n_items() const;

	double total_volume() const;

	/** max(volumen total / (W*D), altura de la pieza mas alta) */
	double height_lower_bound() const;

	std::string name;
	std::string file;
	std::string root;

	double W, D;

	std::vector<Piece> pieces;
	std::vector<ItemType> types;
};

} /* namespace plp */

#endif /* PLP_INSTANCE_H_ */
