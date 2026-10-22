#!/usr/bin/env python3
"""
Cota inferior del numero de ULDs (instancias de Paquay et al. 2018).

uso: lower_bound_uld.py <uld.csv> <instancia.csv> [...]

max(L_vol, L_peso, L_clique):
  L_vol:    volumen total / volumen (utilizable) del ULD mas grande
  L_peso:   peso total / mayor capacidad de peso
  L_clique: tamano de una clique de cajas incompatibles (dos cajas son incompatibles si en
            ningun tipo de ULD caben juntas: con la caja envolvente del ULD, dos cajas caben
            solo si se pueden separar en algun eje, o si su peso supera la capacidad)
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


def main():
    ulds = read(sys.argv[1])
    for f in sys.argv[2:]:
        lb, lv, lw, lc = lower_bound(ulds, read(f))
        print(f"{f} lb {lb} vol {lv} weight {lw} clique {lc}")


if __name__ == '__main__':
    main()
