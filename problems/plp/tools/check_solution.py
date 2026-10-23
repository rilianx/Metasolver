#!/usr/bin/env python3
"""
Verificacion independiente de una solucion (formato ESICUP) para piezas convexas o uniones de convexos.

No usa el codigo C++ del proyecto. Cada pieza es una union de partes convexas (envolventes de scipy):
* si existe benchs/decompositions/<ruta de la pieza>.obj, las partes son sus cascarones cerrados consecutivos
  (o sus grupos o/g);
* si no, la pieza es la envolvente convexa de TODOS los vertices de su archivo OBJ.
La pieza se alinea al minimo de sus vertices (itemOrigin = AlignToMinimum). Para cada par de partes de piezas
distintas se resuelve un LP que busca el punto mas profundo de su interseccion (max delta s.a.
A1 x + delta <= b1, A2 x + delta <= b2, con normales unitarias); si delta > eps las piezas se solapan.

uso: check_solution.py instancia.json solucion.json carpeta_benchs [eps]
requiere: numpy, scipy
"""
import json, sys, os, itertools
import numpy as np
from scipy.spatial import ConvexHull
from scipy.optimize import linprog

inst_file, sol_file, root = sys.argv[1], sys.argv[2], sys.argv[3]
eps = float(sys.argv[4]) if len(sys.argv) > 4 else 1e-6
I = json.load(open(inst_file)); S = json.load(open(sol_file))
W, D = I['container']['size-x'], I['container']['size-y']

def read_obj(path):
    V, F, groups = [], [], [0]
    for l in open(path):
        l = l.split('#')[0].split()
        if not l: continue
        if l[0] == 'v': V.append([float(x) for x in l[1:4]])
        elif l[0] == 'f':
            f = [int(t.split('/')[0]) for t in l[1:]]
            F.append([k-1 if k > 0 else len(V)+k for k in f])
        elif l[0] in ('o', 'g') and len(F) > groups[-1]: groups.append(len(F))
    return np.array(V), F, groups

def shells(F):
    """separa las caras en cascarones cerrados consecutivos (cada arista usada por exactamente dos caras)"""
    out, cnt, start, open_ = [], {}, 0, 0
    for j, f in enumerate(F):
        for k in range(len(f)):
            e = tuple(sorted((f[k], f[(k+1) % len(f)])))
            c = cnt.get(e, 0) + 1; cnt[e] = c
            open_ += 1 if c == 1 else -1 if c == 2 else 0
        if open_ == 0: out.append((start, j+1)); start = j+1; cnt = {}
    return out if start == len(F) else None

def load_parts(rel):
    dec = root + '/decompositions/' + rel.rsplit('.', 1)[0] + '.obj'
    if os.path.exists(dec):
        V, F, groups = read_obj(dec)
        rng = [(groups[k], groups[k+1] if k+1 < len(groups) else len(F)) for k in range(len(groups))] if len(groups) > 1 else shells(F)
        if rng is None: sys.exit('no se pudo separar en partes: ' + dec)
        parts = [V[sorted(set(i for f in F[a:b] for i in f))] for a, b in rng]
    else:
        if not rel.lower().endswith('.obj'): sys.exit('solo archivos OBJ: ' + rel)
        V, F, _ = read_obj(root + '/' + rel)
        parts = [V]
    lo = np.min([P.min(axis=0) for P in parts], axis=0)
    return [P - lo for P in parts]

pieces = {}
for it in I['item-types']:
    pieces[it['path'].split('/')[-1]] = load_parts(it['path'])

placed = []   # por item: lista de partes (vertices transformados)
for it in S['items']:
    M = np.array(it['transformation']['matrix'])
    placed.append([(np.hstack([V, np.ones((len(V), 1))]) @ M)[:, :3] for V in pieces[it['name']]])

outside = max(max(-P.min(), P[:, 0].max() - W, P[:, 1].max() - D) for Q in placed for P in Q)
height = max(P[:, 2].max() for Q in placed for P in Q)

def halfspaces(P):
    E = ConvexHull(P).equations
    n = np.linalg.norm(E[:, :3], axis=1)
    return E[:, :3] / n[:, None], -E[:, 3] / n

H = [[halfspaces(P) for P in Q] for Q in placed]
box = [(np.min([P.min(axis=0) for P in Q], axis=0), np.max([P.max(axis=0) for P in Q], axis=0)) for Q in placed]
worst, pair = 0.0, None
for i, j in itertools.combinations(range(len(placed)), 2):
    if np.any(box[i][1] <= box[j][0]) or np.any(box[j][1] <= box[i][0]): continue
    for (A1, b1) in H[i]:
        for (A2, b2) in H[j]:
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
