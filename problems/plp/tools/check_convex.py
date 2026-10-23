#!/usr/bin/env python3
"""
Verificacion independiente de una solucion (formato ESICUP) para instancias con piezas convexas.

No usa el codigo C++ del proyecto: cada pieza es la envolvente convexa (scipy) de TODOS los vertices de su
archivo OBJ, alineada al minimo (itemOrigin = AlignToMinimum). Para cada par de piezas se resuelve un LP que
busca el punto mas profundo de su interseccion (max delta s.a. A1 x + delta <= b1, A2 x + delta <= b2, con las
normales unitarias); si delta > eps las piezas se solapan.

uso: check_convex.py instancia.json solucion.json carpeta_benchs [eps]
requiere: numpy, scipy
"""
import json, sys, itertools
import numpy as np
from scipy.spatial import ConvexHull
from scipy.optimize import linprog

inst_file, sol_file, root = sys.argv[1], sys.argv[2], sys.argv[3]
eps = float(sys.argv[4]) if len(sys.argv) > 4 else 1e-6
I = json.load(open(inst_file)); S = json.load(open(sol_file))
W, D = I['container']['size-x'], I['container']['size-y']

def load_obj(path):
    V = [list(map(float, l.split()[1:4])) for l in open(path) if l.startswith('v ')]
    return np.array(V)

pieces = {}
for it in I['item-types']:
    if not it['path'].lower().endswith('.obj'): sys.exit('solo archivos OBJ: ' + it['path'])
    V = load_obj(root + '/' + it['path'])
    pieces[it['path'].split('/')[-1]] = V - V.min(axis=0)

placed = []
for it in S['items']:
    M = np.array(it['transformation']['matrix'])
    V = pieces[it['name']]
    placed.append((np.hstack([V, np.ones((len(V), 1))]) @ M)[:, :3])

outside = max(max(-P.min(), P[:, 0].max() - W, P[:, 1].max() - D) for P in placed)
height = max(P[:, 2].max() for P in placed)

def halfspaces(P):
    E = ConvexHull(P).equations
    n = np.linalg.norm(E[:, :3], axis=1)
    return E[:, :3] / n[:, None], -E[:, 3] / n

H = [halfspaces(P) for P in placed]
worst, pair = 0.0, None
for i, j in itertools.combinations(range(len(placed)), 2):
    (A1, b1), (A2, b2) = H[i], H[j]
    A = np.vstack([np.hstack([A1, np.ones((len(A1), 1))]), np.hstack([A2, np.ones((len(A2), 1))])])
    r = linprog(c=[0, 0, 0, -1], A_ub=A, b_ub=np.concatenate([b1, b2]),
                bounds=[(None, None)] * 3 + [(None, 1e3)], method='highs')
    if r.status == 0 and r.x[3] > worst: worst, pair = r.x[3], (i, j)

ok = outside <= eps and worst <= eps
print('items: %d' % len(placed))
print('height: %.10g' % height)
print('max_outside: %.3g' % outside)
print('max_intersection_depth: %.3g%s' % (worst, '' if pair is None else '  (piezas %d y %d)' % pair))
print('feasible: %s' % ('yes' if ok else 'no'))
sys.exit(0 if ok else 2)
