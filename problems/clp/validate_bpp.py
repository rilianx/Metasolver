#!/usr/bin/env python3
"""
Validador independiente de soluciones del 3D-BPP (no comparte codigo con BSG_BPP).
Revisa: cada caja de la instancia cargada exactamente una vez, dimensiones de una orientacion
permitida (fija, o las 6 con --rot), dentro del contenedor y sin intersecciones.
uso: validate_bpp.py <instancia> <solucion> [--rot] [-i k]
(-i: instancia k, desde 1, en los archivos de Martello, Pisinger y Vigo con varias instancias)
Imprime el numero de contenedores o termina con error.
"""
import sys
from itertools import permutations

def main():
    inst, sol = sys.argv[1], sys.argv[2]
    rot = "--rot" in sys.argv
    k = int(sys.argv[sys.argv.index("-i") + 1]) if "-i" in sys.argv else 1
    rows = [list(map(int, l.split())) for l in open(inst) if l.strip()]
    r = 0
    while len(rows[r]) != 4 or (r > 0 and len(rows[r - 1]) == 3 and rows[r - 1][0] != k):
        r += 1 + (rows[r][0] if len(rows[r]) == 4 else 0)
    n, L, W, H = rows[r]
    boxes = [tuple(rows[r + 1 + i]) for i in range(n)]
    lines = open(sol).read().strip().split("\n")[1:]
    seen = [False] * n
    bins = {}
    for ln in lines:
        b, item, x, y, z, l, w, h = map(int, ln.split(";"))
        assert 0 <= item < n and not seen[item], f"item {item} repeated or invalid"
        seen[item] = True
        dims = (l, w, h)
        allowed = set(permutations(boxes[item])) if rot else {boxes[item]}
        assert dims in allowed, f"item {item}: invalid dimensions {dims}"
        assert x >= 0 and y >= 0 and z >= 0 and x + l <= L and y + w <= W and z + h <= H, f"item {item} outside the bin"
        bins.setdefault(b, []).append((x, y, z, x + l, y + w, z + h))
    assert all(seen), "some items are not loaded"
    for b, P in bins.items():
        for i in range(len(P)):
            for j in range(i + 1, len(P)):
                p, q = P[i], P[j]
                if all(p[k] < q[k + 3] and q[k] < p[k + 3] for k in range(3)):
                    raise AssertionError(f"bin {b}: boxes overlap")
    print(len(bins))

if __name__ == "__main__":
    main()
