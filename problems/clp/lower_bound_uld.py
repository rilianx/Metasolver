#!/usr/bin/env python3
"""
Cota inferior del numero de ULDs (instancias de Paquay et al. 2018).

uso: lower_bound_uld.py [--cp UB_FILE] [--time T] <uld.csv> <instancia.csv> [...]

max(L_vol, L_peso, L_clique):
  L_vol:    volumen total / volumen (utilizable) del ULD mas grande
  L_peso:   peso total / mayor capacidad de peso
  L_clique: tamano de una clique de cajas incompatibles (dos cajas son incompatibles si en
            ningun tipo de ULD caben juntas: con la caja envolvente del ULD, dos cajas caben
            solo si se pueden separar en algun eje, o si su peso supera la capacidad)

Con --cp (requiere ortools) se intenta subir la cota a la cantidad de ULDs de una solucion
(UB_FILE: lineas "<instancia> <ULDs>"): se prueba con CP-SAT que no existe una asignacion de
las cajas a UB-1 ULDs (de cualquier tipo) que cumpla, en cada ULD,
  - el volumen y el peso,
  - que cada caja quepa sola en el tipo de ULD y cada par de cajas quepa junto (caja envolvente),
  - en cada eje d: las cajas que en toda orientacion permitida (que quepa en el ULD) miden mas
    de la mitad del ULD en el eje d no pueden superponerse en la proyeccion sobre el plano
    perpendicular a d, por lo que la suma de sus menores proyecciones no supera el area de ese
    plano.
Si es infactible, la cota es UB (la solucion es optima).
"""
import sys
import math
from itertools import permutations


def read(path):
    return [list(map(float, l.strip().split(';')[:22])) for l in open(path) if l.strip()]


def orientations(it):
    # (l, w, h) con la dimension vertical permitida
    res = set()
    for p in permutations(range(3)):
        if it[4 + p[2]] == 1:
            res.add((it[p[0]], it[p[1]], it[p[2]]))
    return res


def fits_alone(it, u):
    return it[3] <= u[3] and any(l <= u[0] and w <= u[1] and h <= u[2] for (l, w, h) in orientations(it))


def fit_together(a, b, u):
    if a[3] + b[3] > u[3]:
        return False
    D = u[0:3]
    for oa in orientations(a):
        if any(oa[d] > D[d] for d in range(3)):
            continue
        for ob in orientations(b):
            if any(ob[d] > D[d] for d in range(3)):
                continue
            if any(oa[d] + ob[d] <= D[d] for d in range(3)):
                return True
    return False


def max_clique(adj, n):
    # ramificacion y acotamiento simple (n <= 100, grafos dispersos)
    best = []
    order = sorted(range(n), key=lambda v: -len(adj[v]))

    def expand(R, P):
        nonlocal best
        if len(R) > len(best):
            best = R[:]
        if len(R) + len(P) <= len(best):
            return
        for v in list(P):
            if len(R) + len(P) <= len(best):
                return
            expand(R + [v], [w for w in P if w in adj[v]])
            P.remove(v)

    expand([], order)
    return best


def lower_bound(ulds, items):
    vmax = max(u[20] for u in ulds)
    wmax = max(u[3] for u in ulds)
    l_vol = math.ceil(sum(i[0] * i[1] * i[2] for i in items) / vmax - 1e-9)
    l_w = math.ceil(sum(i[3] for i in items) / wmax - 1e-9)
    n = len(items)
    adj = [set() for _ in range(n)]
    for a in range(n):
        for b in range(a + 1, n):
            if not any(fits_alone(items[a], u) and fits_alone(items[b], u) and fit_together(items[a], items[b], u)
                       for u in ulds):
                adj[a].add(b)
                adj[b].add(a)
    l_clique = len(max_clique(adj, n)) if n else 0
    return max(l_vol, l_w, l_clique), l_vol, l_w, l_clique


def fitting_orientations(it, u):
    return [o for o in orientations(it) if o[0] <= u[0] and o[1] <= u[1] and o[2] <= u[2]]


def cp_infeasible(ulds, items, k, time_limit):
    """True si se prueba que las cajas no caben en k ULDs (relajacion descrita arriba)"""
    from ortools.sat.python import cp_model
    n = len(items)
    m = cp_model.CpModel()
    bins = [u for u in ulds for _ in range(k)]
    vol = [int(i[0] * i[1] * i[2]) for i in items]
    wt = [int(round(i[3] * 10)) for i in items]
    x = {}
    for j, u in enumerate(bins):
        elig = [a for a in range(n) if fits_alone(items[a], u)]
        for a in elig:
            x[a, j] = m.NewBoolVar('')
        m.Add(sum(vol[a] * x[a, j] for a in elig) <= int(u[20]))
        m.Add(sum(wt[a] * x[a, j] for a in elig) <= int(round(u[3] * 10)))
        for p in range(len(elig)):
            for q in range(p + 1, len(elig)):
                a, b = elig[p], elig[q]
                if not fit_together(items[a], items[b], u):
                    m.AddBoolOr([x[a, j].Not(), x[b, j].Not()])
        D = u[0:3]
        for d in range(3):
            big, area = [], []
            for a in elig:
                ors = fitting_orientations(items[a], u)
                if all(o[d] * 2 > D[d] for o in ors):
                    big.append(a)
                    area.append(int(min(o[0] * o[1] * o[2] / o[d] for o in ors)))
            if len(big) > 1:
                m.Add(sum(ar * x[a, j] for a, ar in zip(big, area)) <= int(D[0] * D[1] * D[2] / D[d]))
    used = [m.NewBoolVar('') for _ in bins]
    for (a, j), v in x.items():
        m.AddImplication(v, used[j])
    for a in range(n):
        m.AddExactlyOne([x[a, j] for j in range(len(bins)) if (a, j) in x])
    m.Add(sum(used) <= k)
    # simetria: los ULDs de cada tipo se usan en orden
    for t in range(len(ulds)):
        for c in range(k - 1):
            m.AddImplication(used[t * k + c + 1], used[t * k + c])
    s = cp_model.CpSolver()
    s.parameters.max_time_in_seconds = time_limit
    s.parameters.num_workers = 1
    return s.Solve(m) == cp_model.INFEASIBLE


def main():
    args = sys.argv[1:]
    ub, tl = None, 30.0
    if args[0] == '--cp':
        ub = {l.split()[0]: int(l.split()[1]) for l in open(args[1]) if l.strip()}
        args = args[2:]
    if args[0] == '--time':
        tl = float(args[1])
        args = args[2:]
    ulds = read(args[0])
    for f in args[1:]:
        items = read(f)
        lb, lv, lw, lc = lower_bound(ulds, items)
        line = f"{f} lb {lb} vol {lv} weight {lw} clique {lc}"
        if ub is not None:
            name = f.split('/')[-1].rsplit('.', 1)[0]
            u = ub.get(name)
            cp = lb
            if u is not None and u > lb and cp_infeasible(ulds, items, u - 1, tl):
                cp = u
            line += f" cp {cp}"
        print(line, flush=True)


if __name__ == '__main__':
    main()
