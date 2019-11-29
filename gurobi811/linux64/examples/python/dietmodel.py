#!/usr/bin/python

# Copyright 2019, Gurobi Optimization, LLC

# Solve the classic diet model.  This file implements
# a function that formulates and solves the model,
# but it contains no model data.  The data is
# passed in by the calling program.  Run example 'diet2.py',
# 'diet3.py', or 'diet4.py' to invoke this function.

from gurobipy import *


def solve(categories, minNutrition, maxNutrition, foods, cost,
          nutritionValues):
    # Model
    m = Model("diet")

    # Create decision variables for the foods to buy
    buy = m.addVars(foods, name="buy")

    # The objective is to minimize the costs
    m.setObjective(buy.prod(cost), GRB.MINIMIZE)

    # Nutrition constraints
    m.addConstrs(
        (quicksum(nutritionValues[f,c] * buy[f] for f in foods)
        	== [minNutrition[c], maxNutrition[c]]
         for c in categories), "_")


    def printSolution():
        if m.status == GRB.Status.OPTIMAL:
            print('\nCost: %g' % m.objVal)
            print('\nBuy:')
            buyx = m.getAttr('x', buy)
            for f in foods:
                if buy[f].x > 0.0001:
                    print('%s %g' % (f, buyx[f]))
        else:
            print('No solution')

    # Solve
    m.optimize()
    printSolution()

    print('\nAdding constraint: at most 6 servings of dairy')
    m.addConstr(buy.sum(['milk','ice cream']) <= 6, "limit_dairy")

    # Solve
    m.optimize()
    printSolution()
