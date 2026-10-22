/*
 * Placer.h
 *
 * Generacion de posiciones factibles para una pieza (sin rotaciones).
 *
 * La pieza A (trasladada en t) y una parte fija B (trasladada en tB) se solapan si y solo si
 * t - tB esta en el interior de NFP(A,B) = B + (-A) (no-fit polytope, convexo para
 * partes convexas). Cada faceta n.x <= d del NFP define una restriccion lineal valida
 * n.(t - tB) >= d. Desde una posicion factible se elige, para cada parte cercana, una faceta
 * que la posicion satisface y se resuelve un LP en t (descenso por LP); se repite hasta que
 * la posicion no mejora.
 */

#ifndef PLP_PLACER_H_
#define PLP_PLACER_H_

#include <vector>
#include <map>
#include <random>
#include "Instance.h"
#include "SmallLP.h"

namespace plp {

/** parte convexa ubicada */
struct PlacedPart {
	int part;   //indice global de la parte
	Vec3 t;
	Box3 bbox;
};

/** parte de una forma: parte convexa (indice global) desplazada en off */
struct ShapePart {
	int part;
	Vec3 off;
};

/**
 * Pseudobloque: dos piezas (tipos k1 y k2) en una posicion relativa compacta.
 * Ubicado en t, la pieza k1 queda en t + o1 y la pieza k2 en t + o2.
 */
struct Block {
	int k1, k2;
	Vec3 o1, o2;
	double volume;
	Box3 bbox;
	//volumen de las piezas / volumen de la caja envolvente del bloque
	double density;
};

class Placer {
public:

	/**
	 * @param grid numero de puntos de partida por eje (x,y) en la cara superior de la carga
	 * @param slide peso de x e y (hacia la pared mas cercana) en la funcion objetivo del descenso;
	 *        el peso de z (gravedad) es 1
	 */
	Placer(const Instance& inst, int grid=3, double slide=0.1, double eps=1e-6);

	/**
	 * Genera pseudobloques de dos piezas: para cada par de tipos se prueban como posiciones relativas los
	 * vertices de los NFP entre sus partes (las piezas se tocan); se conservan las per_pair posiciones
	 * factibles de mayor densidad (volumen / caja envolvente) cuya caja es al menos un fraccion min_gain
	 * menor que la suma de las cajas de las piezas, y en total los max_blocks mejores.
	 * Las formas p >= n_pieces() son los bloques (p - n_pieces()).
	 */
	void generate_blocks(int max_blocks, int per_pair=2, double min_gain=0.05);

	/**
	 * Con seed > 0 los puntos de partida se perturban levemente (1e-7 de la escala). El algoritmo es muy
	 * sensible a perturbaciones pequenas, asi que distintas semillas equivalen a corridas independientes.
	 */
	void set_seed(unsigned seed) { this->seed = seed; rng.seed(seed); }

	int n_pieces() const { return inst.pieces.size(); }

	int n_shapes() const { return shape_parts.size(); }

	const std::vector<Block>& get_blocks() const { return blocks; }

	double shape_volume(int p) const { return shape_vol[p]; }

	Vec3 shape_extent(int p) const { return shape_box[p].extent(); }

	/**
	 * Posiciones factibles para la forma p (pieza o bloque) dadas las partes ubicadas.
	 * top es la altura actual de la carga; hmax es la altura del contenedor
	 * (se descartan las posiciones en que la pieza sobrepasa hmax).
	 */
	void candidates(int p, const std::vector<PlacedPart>& placed, double top, std::vector<Vec3>& out,
			double hmax=std::numeric_limits<double>::infinity());

	/** true si la forma p en t no se solapa con las partes ubicadas (penetracion <= eps) */
	bool feasible(int p, const Vec3& t, const std::vector<PlacedPart>& placed) const;

	/**
	 * Mejora una solucion (piezas pieces[i] en t[i]) moviendo una pieza a la vez con las demas fijas:
	 * - la pieza vuelve a descender por LP desde su posicion (compactacion);
	 * - las piezas que definen la altura se prueban en otras posiciones candidatas.
	 * Repite hasta que la altura no mejora. Retorna la nueva altura.
	 */
	double improve(const std::vector<int>& pieces, std::vector<Vec3>& t, int max_passes=50);

	/**
	 * Compactacion global por LP: variables t_i (todas las piezas) y H; minimiza H.
	 * Para cada par de partes que pueden tocarse dentro de la region de confianza
	 * (|t_i - t_i^0| <= delta por eje) se fija una faceta del NFP: n.(t_i - t_j) >= d.
	 * Se repite (eligiendo de nuevo las facetas) mientras la altura baje.
	 * Retorna la nueva altura. Las posiciones se actualizan solo si la solucion es factible.
	 */
	double compact(const std::vector<int>& pieces, std::vector<Vec3>& t, class LPBackend& lp, int max_iter=30);

	/** partes ubicadas de las piezas, salvo la pieza skip */
	void place_all(const std::vector<int>& pieces, const std::vector<Vec3>& t, int skip, std::vector<PlacedPart>& out) const;

	/** partes (indices globales) de la pieza p */
	const std::vector<int>& get_parts(int p) const { return piece_parts[p]; }

	const ConvexPolytope& get_part(int k) const { return *parts[k]; }

	const Instance& inst;

	/**
	 * Descenso por LP: desde la posicion factible t, minimiza c.t sin solapar las partes ubicadas
	 * (en una region de confianza alrededor de t, eligiendo una faceta de cada NFP cercano)
	 * hasta que la posicion no mejora. Retorna una posicion factible.
	 */
	Vec3 descend(int p, Vec3 t, const Vec3& c, const std::vector<PlacedPart>& placed);

private:
	/** NFP de la parte a (movil) respecto a la parte b (fija), ambas sin trasladar */
	const ConvexPolytope& nfp(int a, int b);

	int grid;
	double slide;
	double eps;
	double tol;

	std::vector<const ConvexPolytope*> parts;
	std::vector< std::vector<int> > piece_parts;

	//formas: piezas de la instancia y luego bloques
	std::vector< std::vector<ShapePart> > shape_parts;
	std::vector<Box3> shape_box;
	std::vector<double> shape_vol;
	std::vector<Block> blocks;

	std::map< std::pair<int,int>, ConvexPolytope > nfps;

	SmallLP lp;

	unsigned seed;
	std::mt19937 rng;
};

} /* namespace plp */

#endif /* PLP_PLACER_H_ */
