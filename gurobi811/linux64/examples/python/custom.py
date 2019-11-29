#
# Copyright 2019, Gurobi Optimization, LLC
#
# Interactive shell customization example
#
# Define a set of customizations for the Gurobi shell.
# Type 'from custom import *' to import them into your shell.
#

from gurobipy import *


# custom read command --- change directory as appropriate

def myread(name):
    return read('/home/jones/models/' + name)


# simple termination callback

def mycallback(model, where):
    if where == GRB.Callback.MIP:
        time = model.cbGet(GRB.Callback.RUNTIME)
        best = model.cbGet(GRB.Callback.MIP_OBJBST)
        if time > 10 and best < GRB.INFINITY:
            model.terminate()


# custom optimize() function that uses callback

def myopt(model):
    model.optimize(mycallback)
