#!/usr/bin/env python3
"""
Mejora de una solucion con un MILP sobre una ventana de piezas (mateheuristica sobre el modelo de
Tollenaere et al. 2026), resuelto con HiGHS. Independiente del codigo C++.

Se eligen las k piezas mas altas (las que definen la altura); las demas quedan fijas. Variables: traslaciones de
las piezas de la ventana y la altura H (se minimiza). Para cada par de partes convexas en que al menos una pieza
esta en la ventana, no-solapamiento con el NFP = envolvente de {v_b - v_a}: para alguna faceta (n, d) del NFP,
n.(t_a - t_b) >= d; una binaria por faceta, suma >= 1, y big-M.

uso: milp_window.py instancia.json solucion.json benchs salida.json [k=4] [tiempo=60]
requiere: numpy, scipy, highspy
"""
import json, sys, os, itertools
import numpy as np
from scipy.spatial import ConvexHull
import highspy

sys.argv += [None] * 3
inst_file, sol_file, root, out_file = sys.argv[1:5]
k = int(sys.argv[5]) if sys.argv[5] else 4
tlim = float(sys.argv[6]) if sys.argv[6] else 60.0
I = json.load(open(inst_file)); S = json.load(open(sol_file))
Wc, Dc = I['container']['size-x'], I['container']['size-y']

# --- lectura de piezas (mismo criterio que check_solution.py) ---
sys.argv = [sys.argv[0], inst_file, sol_file, root]
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'check_solution.py')).read()
exec(src[src.index('def read_obj'):src.index('pieces = {}')])

parts = {}
for it in I['item-types']:
    parts[it['path'].split('/')[-1]] = load_parts(it['path'])
names = [it['name'] for it in S['items']]
T = np.array([np.array(it['transformation']['matrix'])[3, :3] for it in S['items']])
n = len(names)
ext = [np.max([P.max(axis=0) for P in parts[nm]], axis=0) for nm in names]   # caja (minimo en el origen)
top = np.array([T[i, 2] + ext[i][2] for i in range(n)])
H0 = top.max()
win = list(np.argsort(-top)[:k])
print('altura inicial %.6g; ventana: %s' % (H0, win))

nfp_cache = {}
def nfp(na, ia, nb, ib):
    key = (na, ia, nb, ib)
    if key not in nfp_cache:
        A, B = parts[na][ia], parts[nb][ib]
        P = (B[None, :, :] - A[:, None, :]).reshape(-1, 3)
        E = ConvexHull(P).equations
        nn = np.linalg.norm(E[:, :3], axis=1)
        nfp_cache[key] = (E[:, :3] / nn[:, None], -E[:, 3] / nn)
    return nfp_cache[key]

h = highspy.Highs(); h.setOptionValue('output_flag', False); h.setOptionValue('time_limit', tlim)
inf = highspy.kHighsInf
idx = {}
def addvar(lo, hi, cost=0.0, integer=False):
    j = h.getNumCol(); h.addVar(lo, hi); h.changeColCost(j, cost)
    if integer: h.changeColIntegrality(j, highspy.HighsVarType.kInteger)
    return j
# variables de la ventana y altura
for i in win:
    e = ext[i]
    idx[i] = [addvar(0, max(0, Wc - e[0])), addvar(0, max(0, Dc - e[1])), addvar(0, H0 - e[2])]
Hv = addvar(0, H0, 1.0)
for i in win:
    h.addRow(-inf, -ext[i][2], 2, np.array([idx[i][2], Hv], dtype=np.int32), np.array([1.0, -1.0]))

# rango de cada coordenada de la traslacion (para big-M)
def rng(i):
    if i in idx: e = ext[i]; return np.array([[0, max(0, Wc - e[0])], [0, max(0, Dc - e[1])], [0, H0 - e[2]]])
    return np.array([[T[i, c], T[i, c]] for c in range(3)])

nbin = 0
for a, b in itertools.combinations(range(n), 2):
    if a not in idx and b not in idx: continue
    Ra, Rb = rng(a), rng(b)
    for ia in range(len(parts[names[a]])):
        for ib in range(len(parts[names[b]])):
            N, d = nfp(names[a], ia, names[b], ib)
            # rel = t_a - t_b; caja de rel posible
            rlo, rhi = Ra[:, 0] - Rb[:, 1], Ra[:, 1] - Rb[:, 0]
            # si la caja del NFP no intersecta la caja de rel, el par no puede solaparse
            Pb = parts[names[b]][ib]; Pa = parts[names[a]][ia]
            nlo, nhi = Pb.min(axis=0) - Pa.max(axis=0), Pb.max(axis=0) - Pa.min(axis=0)
            if np.any(rhi <= nlo) or np.any(rlo >= nhi): continue
            bins = []
            for f in range(len(N)):
                nf, df = N[f], d[f]
                minrel = np.sum(np.where(nf > 0, nf * rlo, nf * rhi))   # minimo de nf.rel en la caja
                if minrel >= df: bins = None; break                    # la faceta se cumple siempre: el par nunca se solapa
                M = df - minrel
                z = addvar(0, 1, 0.0, True); bins.append(z); nbin += 1
                # nf.(t_a - t_b) - M z >= df - M   (con z=1: nf.rel >= df)
                cols, vals, const = [], [], 0.0
                for c in range(3):
                    if a in idx: cols.append(idx[a][c]); vals.append(nf[c])
                    else: const += nf[c] * T[a, c]
                    if b in idx: cols.append(idx[b][c]); vals.append(-nf[c])
                    else: const -= nf[c] * T[b, c]
                cols.append(z); vals.append(-M)
                h.addRow(df - M - const, inf, len(cols), np.array(cols, dtype=np.int32), np.array(vals))
            if bins:
                h.addRow(1, inf, len(bins), np.array(bins, dtype=np.int32), np.ones(len(bins)))
print('binarias: %d, restricciones: %d' % (nbin, h.getNumRow()))

# solucion inicial: la actual
sol = np.zeros(h.getNumCol())
for i in win:
    for c in range(3): sol[idx[i][c]] = T[i, c]
sol[Hv] = H0
hs = highspy.HighsSolution(); hs.col_value = list(sol)
h.setSolution(hs)
h.run()
st = h.getModelStatus()
info = h.getInfo()
print('estado: %s, objetivo %.6g, cota %.6g' % (h.modelStatusToString(st), info.objective_function_value, info.mip_dual_bound))
x = np.array(h.getSolution().col_value)
if len(x) == 0 or info.objective_function_value >= H0 - 1e-9:
    print('sin mejora'); sys.exit(1)
for i in win:
    M = np.array(S['items'][i]['transformation']['matrix'], dtype=float)
    M[3, :3] = [x[idx[i][c]] for c in range(3)]
    S['items'][i]['transformation']['matrix'] = M.tolist()
    S['items'][i]['transformation']['position'] = M[3, :3].tolist()
S['totalHeight'] = float(max(np.array(it['transformation']['matrix'])[3, 2] + ext[j][2] for j, it in enumerate(S['items'])))
json.dump(S, open(out_file, 'w'), indent=1)
print('nueva altura: %.6g -> %s' % (S['totalHeight'], out_file))
