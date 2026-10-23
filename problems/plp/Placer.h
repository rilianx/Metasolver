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
	//hueco agregado: (envolvente convexa del bloque - envolventes de las piezas) / volumen de las piezas
	//(negativo si una pieza entra en una concavidad de la otra)
	double waste;
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
	 * Genera pseudobloques de dos piezas. Para cada par de tipos, las posiciones relativas candidatas son
	 * los vertices de los NFP entre sus partes y los puntos de sus facetas sobre los ejes (las piezas se
	 * tocan); se evaluan las max_eval factibles de menor caja envolvente. Un bloque se acepta si su hueco
	 * agregado (envolvente convexa del bloque - envolventes de las piezas, relativo al volumen de las piezas)
	 * es a lo mas max_waste. Se conservan los per_pair de menor hueco por par y los max_blocks mejores en total.
	 * Con by_box=true el criterio es el de la caja envolvente: se acepta si la caja del bloque es al menos una
	 * fraccion min_gain menor que la suma de las cajas de las piezas, se toman por par los per_pair factibles
	 * de menor caja y en total los de mayor densidad (volumen / caja).
	 * Las formas p >= n_pieces() son los bloques (p - n_pieces()).
	 */
	void generate_blocks(int max_blocks, int per_pair=2, double max_waste=0.1, int max_eval=30,
			bool by_box=false, double min_gain=0.05);

	/** numero de piezas de la forma p (1 para piezas, 2 para pseudobloques) */
	int shape_pieces(int p) const { return shape_npieces[p]; }

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

	/**
	 * Separacion por LP con altura fija H (las posiciones t pueden tener solapamientos).
	 * Variables: t_i y una holgura s_k >= 0 por par de partes cercanas, con n.(t_i - t_j) + s_k >= d para una
	 * faceta del NFP (la de mayor holgura en la posicion actual); minimiza sum w_ij s_k, donde w_ij es el peso
	 * del par de piezas (busqueda local guiada: aumenta para los pares que siguen solapados).
	 * Se repite eligiendo de nuevo las facetas. Retorna true si se elimina el solapamiento (t queda factible).
	 * Si weights no es NULL, los pesos se leen y se actualizan ahi (persisten entre llamadas).
	 */
	bool separate(const std::vector<int>& pieces, std::vector<Vec3>& t, double H, class LPBackend& lp, int max_iter=50,
			std::map< std::pair<int,int>, double >* weights=NULL);

	/**
	 * Saca la pieza i y la reubica con el descenso por LP en el espacio libre del contenedor de altura H
	 * (las demas piezas fijas). Retorna false si no hay posicion factible.
	 */
	bool reinsert(const std::vector<int>& pieces, std::vector<Vec3>& t, int i, double H);

	/**
	 * Movimiento de minimo solapamiento (Egeblad et al. 2009; Umetani et al. 2009): traslada la pieza i a lo
	 * largo del eje axis (0,1,2) a la posicion que minimiza sum_j w_ij * pen_ij, donde pen_ij es la suma, sobre
	 * los pares de partes, de la profundidad de penetracion max(0, min_f (d_f - n_f.(t_i - t_j))) en su NFP.
	 * A lo largo de la recta cada termino es concavo en un intervalo y cero fuera, asi que el minimo esta en los
	 * extremos de esos intervalos o del contenedor (altura H). Retorna la mejora del objetivo (>= 0).
	 */
	double axis_move(const std::vector<int>& pieces, std::vector<Vec3>& t, int i, int axis, double H,
			const std::map< std::pair<int,int>, double >& w);

	/**
	 * Busqueda local de minimo solapamiento con altura fija H: mueve las piezas solapadas por los tres ejes
	 * (axis_move) y, en los minimos locales, aumenta los pesos de los pares que siguen solapados (busqueda local
	 * guiada; los pesos persisten en weights). Retorna true si el solapamiento se elimina (verificado con overlap()).
	 *
	 * mult: pesos multiplicativos (Gardeyn et al. 2025, sparrow): en un minimo local el peso m_ij = 1 + w_ij de un par
	 * solapado se multiplica por 1.2 + 0.8*pen_ij/max pen y el de los demas pares decae (x0.95, minimo 1); si no,
	 * se suma 1 al peso de los pares solapados.
	 * strike_iter > 0: se guarda la mejor disposicion (menor penetracion total sin pesos); tras strike_iter pasadas
	 * sin mejorarla se vuelve a ella (strike) y tras max_strikes se termina. Al fallar, t queda en la mejor disposicion.
	 */
	bool minimize_overlap(const std::vector<int>& pieces, std::vector<Vec3>& t, double H, int max_iter,
			std::map< std::pair<int,int>, double >& weights, std::mt19937& rng, bool mult=false,
			int strike_iter=0, int max_strikes=3);

	/**
	 * Intercambio de las piezas i y j (Imamichi et al. 2009; Gardeyn et al. 2025): cada una toma el centro de la caja
	 * de la otra (dentro del contenedor de altura H).
	 */
	void swap_pieces(const std::vector<int>& pieces, std::vector<Vec3>& t, int i, int j, double H) const;

	/** penetracion entre las piezas i y j (suma sobre sus pares de partes, con los NFP) */
	double pair_penetration(const std::vector<int>& pieces, const std::vector<Vec3>& t, int i, int j);

	/** suma, sobre los pares de piezas, de la mayor profundidad de penetracion entre sus partes */
	double overlap(const std::vector<int>& pieces, const std::vector<Vec3>& t, std::vector< std::pair<int,int> >* pairs=NULL) const;

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
	std::vector<int> shape_npieces;
	std::vector<Block> blocks;

	std::map< std::pair<int,int>, ConvexPolytope > nfps;

	SmallLP lp;

	unsigned seed;
	std::mt19937 rng;
};

} /* namespace plp */

#endif /* PLP_PLACER_H_ */
