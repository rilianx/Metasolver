// Copyright (C) 2019, Gurobi Optimization, LLC
// All Rights Reserved
#ifndef _COLUMN_CPP_H_
#define _COLUMN_CPP_H_


class GRBColumn
{
  private:

    std::vector<double> coeffs;
    std::vector<GRBConstr> constrs;

  public:

    unsigned int size(void) const;
    GRBConstr getConstr(int i) const;
    double getCoeff(int i) const;

    void addTerm(double coeff, GRBConstr constr);
    void addTerms(const double* coeff, const GRBConstr* constr, int cnt);
    void remove(int i);
    bool remove(GRBConstr c);

    void clear();
};
#endif
