#!/usr/bin/env python3
"""
Prototipo: dado el archivo de --columns de BSG_BPP (contenedores generados), elige con CP-SAT
el menor numero de contenedores que cubren todas las cajas (multicobertura: sobrar cajas
esta permitido, porque sacar cajas de un contenedor mantiene la factibilidad).
uso: setcover_bpp.py <columns> [tiempo]
"""
import sys
from ortools.linear_solver import pywraplp
lines = open(sys.argv[1]).read().strip().split("\n")
h = list(map(int, lines[0].split()))
T, lb, demand = h[0], h[1], h[2:]
cols = sorted({tuple(map(int, l.split())) for l in lines[1:]})
s = pywraplp.Solver.CreateSolver("SCIP")
x = [s.IntVar(0, s.infinity(), f"x{j}") for j in range(len(cols))]
for t in range(T):
    s.Add(sum(c[t] * x[j] for j, c in enumerate(cols) if c[t]) >= demand[t])
s.Minimize(sum(x))
s.SetTimeLimit(int(1000 * (float(sys.argv[2]) if len(sys.argv) > 2 else 10)))
r = s.Solve()
print(int(round(s.Objective().Value())), round(s.Objective().BestBound(), 2), r, len(cols))
