// Copyright (C) 2019, Gurobi Optimization, LLC
// All Rights Reserved
#ifndef _GENCONSTR_CPP_H_
#define _GENCONSTR_CPP_H_


class GRBGenConstrRep // private one
{
  private:
    GRBmodel*  Cmodel;
    int        num;
  public:
    friend class GRBGenConstr;
};

class GRBGenConstr
{
  private:

    GRBGenConstrRep* genconRep;

    GRBGenConstr(GRBmodel* xmodel, int genc);
    void setindex(int genc);
    int  getindex() const;
    void remove();

  public:

    friend class GRBModel;

    GRBGenConstr();
    int get(GRB_IntAttr attr) const;
    std::string get(GRB_StringAttr attr) const;

    void set(GRB_StringAttr attr, const std::string& value);
};
#endif
