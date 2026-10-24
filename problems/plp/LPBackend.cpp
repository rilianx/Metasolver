/*
 * LPBackend.cpp
 *
 * Implementacion con SoPlex (https://soplex.zib.de) si PLP_WITH_SOPLEX esta definido. Los parametros son los que
 * usaba la version anterior a traves de ibex: simplex en aritmetica real, tolerancias de factibilidad y optimalidad
 * 1e-9, limite de iteraciones y sin reloj (con el reloj de CPU SoPlex hace una llamada al sistema en cada
 * resolucion de la LU).
 */

#include <limits>
#include <map>
#include "LPBackend.h"

#ifdef PLP_WITH_SOPLEX
#include "soplex.h"
#endif

using namespace std;

namespace plp {

#ifdef PLP_WITH_SOPLEX

class SoplexLP : public LPBackend {
public:
	SoplexLP(double timeout) : timeout(timeout), lp(NULL), n(0) { }

	virtual ~SoplexLP() { delete lp; }

	virtual void reset(int nv) {
		n = nv;
		delete lp;
		lp = new soplex::SoPlex();
		lp->setIntParam(soplex::SoPlex::VERBOSITY, soplex::SoPlex::VERBOSITY_ERROR);
		lp->setIntParam(soplex::SoPlex::ITERLIMIT, 1000000);
		lp->setRealParam(soplex::SoPlex::TIMELIMIT, timeout);
		lp->setRealParam(soplex::SoPlex::FEASTOL, 1e-9);
		lp->setRealParam(soplex::SoPlex::OPTTOL, 1e-9);
		lp->setIntParam(soplex::SoPlex::SOLVEMODE, soplex::SoPlex::SOLVEMODE_REAL);
		lp->setIntParam(soplex::SoPlex::OBJSENSE, soplex::SoPlex::OBJSENSE_MINIMIZE);
		lp->setIntParam(soplex::SoPlex::TIMER, soplex::SoPlex::TIMER_OFF);
		inf = lp->realParam(soplex::SoPlex::INFTY);
		soplex::LPColSetReal cols(n, 0);
		soplex::DSVectorReal empty(0);
		for(int i=0; i<n; i++) cols.add(0.0, -inf, empty, inf);
		lp->addColsReal(cols);
	}

	virtual void set_bounds(int var, double lo, double hi) {
		lp->changeBoundsReal(var, clip(lo), clip(hi));
	}

	virtual void set_cost(int var, double c) { lp->changeObjReal(var, c); }

	virtual void add_row(const vector< pair<int,double> >& row, double lo, double hi) {
		map<int,double> r;   //indices repetidos: se suman los coeficientes
		for(const auto& e : row) r[e.first] += e.second;
		soplex::DSVectorReal v((int) r.size());
		for(const auto& e : r) if(e.second != 0.0) v.add(e.first, e.second);
		lp->addRowReal(soplex::LPRowReal(clip(lo), v, clip(hi)));
	}

	virtual bool minimize() {
		try{
			return lp->optimize() == soplex::SPxSolver::OPTIMAL && lp->hasSol();
		}catch(...){   //SoPlex puede lanzar excepciones internas (p.ej., SPxInternalCodeException)
			return false;
		}
	}

	virtual vector<double> solution() const {
		vector<double> s(n, 0.0);
		if(n > 0 && !lp->getPrimalReal(s.data(), n)) s.assign(n, 0.0);
		return s;
	}

	virtual string name() const { return "soplex"; }

private:
	double clip(double v) const { return v >= inf ? inf : v <= -inf ? -inf : v; }

	double timeout;
	soplex::SoPlex* lp;
	int n;
	double inf;
};

LPBackend* LPBackend::create(double timeout) { return new SoplexLP(timeout); }

#else

LPBackend* LPBackend::create(double) { return NULL; }

#endif

} /* namespace plp */
