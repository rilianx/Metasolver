/*
 * LPBackend.cpp
 *
 * Implementacion con ibex (LPSolver, que utiliza SoPlex o CLP) si PLP_WITH_IBEX esta definido.
 */

#include <limits>
#include "LPBackend.h"

#ifdef PLP_WITH_IBEX
#include "ibex.h"
#endif

using namespace std;

namespace plp {

#ifdef PLP_WITH_IBEX

class IbexLP : public LPBackend {
public:
	IbexLP(double timeout) : timeout(timeout), lp(NULL), n(0) { }

	virtual ~IbexLP() { delete lp; }

	virtual void reset(int nv) {
		n = nv;
		delete lp;
		lp = new ibex::LPSolver(n, ibex::LPSolver::Mode::NotCertified, 1e-9, timeout, 1000000);
		bounds = ibex::IntervalVector(n, ibex::Interval::all_reals());
		cost = ibex::Vector(n, 0.0);
	}

	virtual void set_bounds(int var, double lo, double hi) {
		bounds[var] = ibex::Interval(lo, hi);
	}

	virtual void set_cost(int var, double c) { cost[var] = c; }

	virtual void add_row(const vector< pair<int,double> >& row, double lo, double hi) {
		ibex::Vector r(n, 0.0);
		for(const auto& e : row) r[e.first] += e.second;
		lp->add_constraint(lo, r, hi);
	}

	virtual bool minimize() {
		lp->set_bounds(bounds);
		lp->set_cost(cost);
		ibex::LPSolver::Status st = lp->minimize();
		return st==ibex::LPSolver::Status::Optimal || st==ibex::LPSolver::Status::OptimalProved;
	}

	virtual vector<double> solution() const {
		ibex::Vector x = lp->not_proved_primal_sol();
		vector<double> s(n);
		for(int i=0; i<n; i++) s[i]=x[i];
		return s;
	}

	virtual string name() const { return "ibex/" + string(_IBEX_LP_LIB_); }

private:
	double timeout;
	ibex::LPSolver* lp;
	int n;
	ibex::IntervalVector bounds{1};
	ibex::Vector cost{1};
};

LPBackend* LPBackend::create(double timeout) { return new IbexLP(timeout); }

#else

LPBackend* LPBackend::create(double) { return NULL; }

#endif

} /* namespace plp */
