#!/usr/bin/python

# Copyright 2019, Gurobi Optimization, LLC

# Want to cover three different sets but subject to a common budget of
# elements allowed to be used. However, the sets have different priorities to
# be covered; and we tackle this by using multi-objective optimization.

from __future__ import print_function
from gurobipy import *

try:
    # Sample data
    Groundset = range(20)
    Subsets   = range(4)
    Budget    = 12;
    Set = [ [ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
            [ 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1 ],
            [ 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0 ],
            [ 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0 ] ]
    SetObjPriority = [  3,    2,    2,   1]
    SetObjWeight   = [1.0, 0.25, 1.25, 1.0]

    # Create initial model
    model = Model('multiobj')

    # Initialize decision variables for ground set:
    # x[e] == 1 if element e is chosen for the covering.
    Elem = model.addVars(Groundset, vtype=GRB.BINARY, name='El')

    # Constraint: limit total number of elements to be picked to be at most
    # Budget
    model.addConstr(Elem.sum() <= Budget, name='Budget')

    # Set global sense for ALL objectives
    model.ModelSense = GRB.MAXIMIZE

    # Limit how many solutions to collect
    model.setParam(GRB.Param.PoolSolutions, 100)

    # Set and configure i-th objective
    for i in Subsets:
        objn = sum(Elem[k]*Set[i][k] for k in range(len(Elem)))
        model.setObjectiveN(objn, i, SetObjPriority[i], SetObjWeight[i],
                            1.0 + i, 0.01, 'Set' + str(i))

    # Save problem
    model.write('multiobj.lp')

    # Optimize
    model.optimize()

    model.setParam(GRB.Param.OutputFlag, 0)

    # Status checking
    status = model.Status
    if status == GRB.Status.INF_OR_UNBD or \
       status == GRB.Status.INFEASIBLE  or \
       status == GRB.Status.UNBOUNDED:
        print('The model cannot be solved because it is infeasible or unbounded')
        sys.exit(1)

    if status != GRB.Status.OPTIMAL:
        print('Optimization was stopped with status ' + str(status))
        sys.exit(1)

    # Print best selected set
    print('Selected elements in best solution:')
    for e in Groundset:
        if Elem[e].X > 0.9:
            print(' El%d' % e, end='')
    print('')

    # Print number of solutions stored
    nSolutions = model.SolCount
    print('Number of solutions found: ' + str(nSolutions))

    # Print objective values of solutions
    if nSolutions > 10:
        nSolutions = 10
    print('Objective values for first ' + str(nSolutions) + ' solutions:')
    for i in Subsets:
        model.setParam(GRB.Param.ObjNumber, i)
        print('\tSet%d' % i, end='')
        for e in range(nSolutions):
            model.setParam(GRB.Param.SolutionNumber, e)
            print(' %6g' % model.ObjNVal, end='')
        print('')

except GurobiError as e:
    print('Error code ' + str(e.errno) + ": " + str(e))

except AttributeError as e:
    print('Encountered an attribute error: ' + str(e))
