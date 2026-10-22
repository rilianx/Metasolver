/*
 * clpState.h
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */

#ifndef CLPSTATE_H_
#define CLPSTATE_H_

#include <string>
#include <vector>
#include <list>
#include <cmath>

#include "State.h"
#include "BoxShape.h"
#include "Block.h"
#include "Container.h"
#include "ULD.h"

using namespace std;
using namespace metasolver;

namespace clp {

class VCS_Function;

/**
 * Accion: ubicar un bloque en un espacio libre, en la posicion (x,y,z)
 * (la esquina de anclaje del espacio, salvo que haya que desplazarlo por los
 * recortes del ULD)
 */
class clpAction : public Action{
public:
	clpAction(const Block& block, const Space& space, long x, long y, long z) :
		block(block), space(space), x(x), y(y), z(z) { }

	virtual Action* clone() const{ return new clpAction(*this); }

	const Block& block;
	const Space space;
	const long x, y, z;
};

/**
 * Solucion parcial: el contenedor con los bloques ubicados, la cantidad de cajas
 * restantes de cada tipo y la lista de bloques que aun se pueden construir.
 */
class clpState : public State {
public:

	clpState(const clpState& S) : State(S), cont(S.cont), left(S.left),
		valid_blocks(S.valid_blocks), weight_left(S.weight_left), region_weight(S.region_weight),
		min_l(S.min_l), min_w(S.min_w), min_h(S.min_h), hash(S.hash) { }

	virtual State* clone() const{ return new clpState(*this); }

	/**
	 * Volumen ocupado (proporcion). En un ULD, si cog_penalty>0, se divide por
	 * 1 + cog_penalty * (desviacion del centro de gravedad que excede la permitida)
	 * (una carga desbalanceada sigue siendo mejor que el ULD vacio).
	 * Con varios ULDs: volumen cargado respecto al de todos los ULDs, dividido por
	 * 1 + cog_penalty * (suma de las desviaciones), menos open_penalty * (volumen de los ULDs
	 * usados respecto al de todos)
	 */
	virtual double get_value() const;

	/**
	 * Desplazamiento (dx,dy) de toda la carga (del ULD de la region r) que minimiza la desviacion del centro de
	 * gravedad (respetando los limites y recortes del ULD). Retorna la desviacion que
	 * excede la permitida despues de desplazar, relativa a la permitida, sumada en x, y y z
	 * (en z, la altura del CoG que excede la maxima permitida)
	 */
	double cog_violation(long& dx, long& dy, int r=0) const;

	/**
	 * Desplaza toda la carga en (dx,dy). Los espacios libres no se actualizan
	 * (solo se usa en la solucion final)
	 */
	void translate(long dx, long dy, int r=0);

	virtual int get_best_actions(ActionEvaluator* evl, list< Action* >& bactions, int n) const;

	int get_n_valid_blocks() const {return valid_blocks.size();}

	/**
	 * Posicion del bloque b en el espacio sp: la esquina de anclaje, desplazada si es
	 * necesario para cumplir los recortes del ULD. Con soporte, el bloque se ubica en
	 * la base del espacio y debe quedar completamente apoyado.
	 * Retorna false si el bloque no se puede ubicar en el espacio (supported indica si
	 * fue por falta de apoyo).
	 */
	bool locate(const Block& b, const Space& sp, long& x, long& y, long& z, bool* supported=NULL) const;

	/**
	 * Evalua los bloques en el espacio isp y agrega los mejores al ranking de acciones.
	 * Retorna 0 si el espacio se elimino o quedo sin considerar
	 */
	int eval_space(int isp, VCS_Function& vcs, int n) const;

	/** el espacio sp (sin apoyo) queda sin considerar hasta la siguiente insercion */
	bool dormant_space(const Space& sp) const;

	/**
	 * Verifica la solucion: bloques dentro del contenedor (y de los recortes), sin
	 * intersecciones, sin usar mas cajas que las disponibles, sin superar el peso
	 * maximo y, con soporte, con todos los bloques apoyados
	 */
	bool is_feasible() const;

	/** peso total de los bloques ubicados */
	double get_weight() const;

	/** centro de gravedad de la carga (se asume el peso de cada bloque en su centro) */
	void get_cog(double& x, double& y, double& z, int r=0) const;

	/**
	 * Hash del conjunto de bloques ubicados (independiente del orden en que se ubicaron).
	 * Los estados con los mismos bloques en las mismas posiciones son equivalentes.
	 */
	virtual uint64_t get_hash() const { return hash; }

	//tipos de caja de la instancia
	static vector<const BoxShape*> types;

	//cantidad de cajas de cada tipo en la instancia
	static vector<int> initial_boxes;

	//los espacios en los que no cabe ningun bloque se eliminan al buscar acciones
	mutable Container cont;

	//cantidad restante de cajas de cada tipo
	vector<int> left;

	vector<const Block*> valid_blocks;

	//capacidad de peso restante
	double weight_left;

	//varios ULDs: capacidad de peso restante de cada ULD (weight_left es la mayor)
	vector<double> region_weight;

	//ULDs que se cargan a la vez (cada uno en una region del contenedor; vacio: un ULD)
	static vector<const ULD*> region_ulds;

	//varios ULDs: penalizacion del volumen de los ULDs usados en get_value
	static double open_penalty;

	//ULD (recortes y peso maximo) o NULL (contenedor sin restricciones adicionales)
	static const ULD* uld;

	//los bloques se deben apoyar completamente sobre el piso o sobre bloques con
	//la cara superior completa y sin cajas fragiles
	static bool support;

	//criterio de soporte de Paquay et al. (2018) y Hessler et al. (2024): las cuatro
	//esquinas inferiores de cada caja se apoyan en cajas no fragiles o en el piso
	//(y ninguna caja queda sobre una caja fragil). Requiere support=true (los
	//bloques se generan con soporte completo)
	static bool corner_support;

	//con corner_support, una caja tambien esta apoyada si parte de su base se apoya
	//directamente y al menos min_overlap de su base se apoya directa o indirectamente
	//(sobre relleno, en cajas no fragiles hasta padding bajo la base; Hessler et al. 2024)
	static double min_overlap;
	static long padding;

	//los espacios sin apoyo quedan sin considerar hasta la siguiente insercion (si no,
	//se eliminan)
	static bool dormant_spaces;

	/** cara superior de una caja (o del piso elevado) bajo la base de un bloque */
	struct Face {
		long x1, y1, x2, y2;
		bool fragile;
		//a la altura de la base (si no, apoyo indirecto sobre relleno)
		bool direct;
	};

	/** la base [x1,x2] x [y1,y2] de una caja esta apoyada en las caras faces */
	static bool box_supported(long x1, long y1, long x2, long y2, const vector<Face>& faces);

	//penalizacion de la desviacion del centro de gravedad en get_value
	static double cog_penalty;

	friend clpState* new_state(string file, int instance, double min_fr, int max_bl);
	friend clpState* new_state(const ULD& u, const vector<int>& left, double min_fr, int max_bl);
	friend clpState* new_state(const vector<const ULD*>& us, const vector<int>& left, double min_fr, int max_bl);

protected:

	virtual void _transition(const Action& action);

private:

	clpState(long L, long W, long H) : cont(L, W, H), weight_left(HUGE_VAL),
		min_l(L), min_w(W), min_h(H), hash(0x9e3779b97f4a7c15ull) { }


	/**
	 * Elimina de valid_blocks los bloques que no se pueden construir con las cajas restantes
	 */
	void update_valid_blocks();

	void update_min_dim();

	//menor dimension de las cajas restantes (los espacios menores se descartan)
	long min_l, min_w, min_h;

	uint64_t hash;
};

/**
 * Lee la instancia (formato BR) y genera los bloques
 */
clpState* new_state(string file, int instance, double min_fr=0.98, int max_bl=10000);

/**
 * Estado inicial para cargar el ULD u con las cajas left de los tipos clpState::types
 * (se fijan clpState::uld e initial_boxes y se generan los bloques)
 */
clpState* new_state(const ULD& u, const vector<int>& left, double min_fr=0.98, int max_bl=10000);

/**
 * Estado inicial para cargar a la vez los ULDs us (uno al lado del otro en x, cada uno en una
 * region del contenedor) con las cajas left (se fijan clpState::region_ulds e initial_boxes)
 */
clpState* new_state(const vector<const ULD*>& us, const vector<int>& left, double min_fr=0.98, int max_bl=10000);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
