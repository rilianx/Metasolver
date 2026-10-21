/*
 * CPSAT.h
 *
 * Interfaz minima con el solver CP-SAT de OR-Tools.
 *
 * Se usa la API en C de la biblioteca (SolveCpModelWithParameters), que recibe el
 * modelo (CpModelProto) y los parametros (SatParameters) serializados en el formato
 * de protobuf y retorna la respuesta (CpSolverResponse) serializada. La serializacion
 * se hace aqui para el subconjunto de mensajes que se utiliza, por lo que no se
 * necesitan los headers de OR-Tools ni de protobuf: la biblioteca se carga en
 * tiempo de ejecucion (dlopen), p.ej. la que incluye el paquete de python
 * (pip install ortools): .../site-packages/ortools/.libs/libortools.so.9
 */

#ifndef CPSAT_H_
#define CPSAT_H_

#include <string>
#include <vector>
#include <cstdint>

namespace clp {

class CpModel {
public:
	/** nueva variable entera con dominio [lb,ub]; retorna su indice */
	int new_var(int64_t lb, int64_t ub);

	int new_bool() { return new_var(0, 1); }

	/** literal negado */
	static int neg(int lit) { return -lit-1; }

	/** lb <= sum coeffs[i]*vars[i] <= ub, si todos los literales de enforcement son verdaderos */
	void add_linear(const std::vector<int>& vars, const std::vector<int64_t>& coeffs,
			int64_t lb, int64_t ub, const std::vector<int>& enforcement=std::vector<int>());

	/** al menos un literal verdadero, si todos los literales de enforcement son verdaderos */
	void add_bool_or(const std::vector<int>& literals, const std::vector<int>& enforcement=std::vector<int>());

	void add_exactly_one(const std::vector<int>& literals);

	/**
	 * intervalo [start, start+size) (start es una variable y size una constante),
	 * opcional segun el literal presence. Retorna su indice (para add_cumulative)
	 */
	int add_interval(int start, int64_t size, int presence);

	/** intervalo fijo [start, start+size) */
	int add_fixed_interval(int64_t start, int64_t size);

	/** la suma de las demandas de los intervalos que contienen un punto no supera capacity */
	void add_cumulative(int64_t capacity, const std::vector<int>& intervals, const std::vector<int64_t>& demands);

	/** maximizar sum coeffs[i]*vars[i] */
	void maximize(const std::vector<int>& vars, const std::vector<int64_t>& coeffs);

	/** solucion inicial sugerida */
	void add_hint(int var, int64_t value);

	int num_vars() const { return domains.size()/2; }

	/** CpModelProto serializado */
	std::string serialize() const;

private:
	std::vector<int64_t> domains;
	std::vector<std::string> constraints;
	std::vector<int> obj_vars;
	std::vector<int64_t> obj_coeffs;
	std::vector<int> hint_vars;
	std::vector<int64_t> hint_values;
};

class CpSolver {
public:
	enum Status{UNKNOWN=0, MODEL_INVALID=1, FEASIBLE=2, INFEASIBLE=3, OPTIMAL=4};

	/** carga la biblioteca de OR-Tools; termina el programa si no se puede */
	CpSolver(const std::string& library);

	/**
	 * Resuelve el modelo. Retorna el estado y deja en solution el valor de cada variable
	 * (si se encontro una solucion)
	 */
	Status solve(const CpModel& model, double max_time, int num_workers, std::vector<int64_t>& solution);

	/** valor objetivo y cota de la ultima llamada a solve */
	double get_objective() const { return objective; }
	double get_bound() const { return bound; }

private:
	typedef void (*SolveFunction)(const void*, int, const void*, int, void**, int*);
	SolveFunction solve_function;
	double objective, bound;
};

} /* namespace clp */

#endif /* CPSAT_H_ */
