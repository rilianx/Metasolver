#!/usr/bin/env python3
"""
Validador independiente de soluciones de BSG_ULD (instancias de Paquay et al. 2018).

uso: validate_uld.py <instancia.csv> <uld.csv> <solucion.csv> [--support corners|full|none]

Verifica: cada caja cargada exactamente una vez; orientacion permitida; caja dentro del
ULD y de sus recortes; sin intersecciones; peso maximo; soporte (las cuatro esquinas
inferiores de cada caja sobre cajas no fragiles o el piso, o la base completa); nada sobre
cajas fragiles. Cuenta los ULDs con el centro de gravedad fuera de la desviacion permitida
en x o y (cog_violations, como Hessler et al. 2024) y los con el centro de gravedad sobre la
altura maxima (cog_z_violations, Paquay et al. 2016).
"""
import sys
from itertools import permutations


def read_csv(path):
    rows = []
    for line in open(path):
        f = [x for x in line.strip().split(';') if x != '']
        if f:
            rows.append(f)
    return rows


def main():
    args = sys.argv[1:]
    support = 'corners'
    if '--support' in args:
        i = args.index('--support')
        support = args[i + 1]
        del args[i:i + 2]
    inst, uldf, solf = args

    # cajas: l;w;h;peso;l vertical;w vertical;h vertical;fragil;muy densa
    items = [list(map(float, r)) for r in read_csv(inst)]

    # ULDs: L;W;H;peso;costo;dev x,y,z;4 recortes (a,b,c: a*z + b*x = c);volumen;tipo
    ulds = {}
    for r in read_csv(uldf):
        f = list(map(float, r))
        L, W, H = f[0:3]
        cuts = []
        for k in range(4):
            a, b, c = f[8 + 3 * k:11 + 3 * k]
            if a == 0 and b == 0 and c == 0:
                continue
            # lado del semiplano que contiene al centro del ULD
            sign = 1 if a * H / 2 + b * L / 2 <= c else -1
            cuts.append((a, b, c, sign))
        ulds[int(f[21])] = dict(L=L, W=W, H=H, cap=f[3], cost=f[4], dev=f[5:8], cuts=cuts, vol=f[20])

    sol = read_csv(solf)[1:]
    errors = []
    used = {}
    loads = {}
    for r in sol:
        u, ut, it = int(r[0]), int(r[1]), int(r[2])
        x, y, z, l, w, h = map(float, r[3:9])
        if it in used:
            errors.append(f"item {it} loaded twice")
        used[it] = u
        loads.setdefault(u, (ut, []))[1].append((it, x, y, z, l, w, h))

    for it in range(len(items)):
        if it not in used:
            errors.append(f"item {it} not loaded")

    n_cog = 0
    n_cog_z = 0
    cost = 0
    util = []
    for u, (ut, boxes) in sorted(loads.items()):
        U = ulds[ut]
        cost += U['cost']
        weight = 0.0
        vol = 0.0
        mx = my = mz = 0.0
        for (it, x, y, z, l, w, h) in boxes:
            I = items[it]
            weight += I[3]
            vol += l * w * h
            mx += I[3] * (x + l / 2)
            my += I[3] * (y + w / 2)
            mz += I[3] * (z + h / 2)
            # orientacion: (l,w,h) es una permutacion de las dimensiones y la vertical esta permitida
            ok = any(I[p[0]] == l and I[p[1]] == w and I[p[2]] == h and I[4 + p[2]] == 1
                     for p in permutations(range(3)))
            if not ok:
                errors.append(f"ULD {u}: item {it} with invalid orientation {l}x{w}x{h}")
            # dentro del ULD
            if x < 0 or y < 0 or z < 0 or x + l > U['L'] or y + w > U['W'] or z + h > U['H']:
                errors.append(f"ULD {u}: item {it} outside the ULD")
            # recortes (el ULD es convexo: basta revisar las esquinas en el plano x-z)
            for (a, b, c, sign) in U['cuts']:
                for (px, pz) in [(x, z), (x + l, z), (x, z + h), (x + l, z + h)]:
                    if sign * (a * pz + b * px) > sign * c + 1e-6:
                        errors.append(f"ULD {u}: item {it} outside the cut ({a},{b},{c})")
                        break

        # intersecciones, soporte y cajas fragiles
        for i, (it, x, y, z, l, w, h) in enumerate(boxes):
            corners = [False] * 4
            area = 0.0
            pts = [(x, y), (x + l, y), (x, y + w), (x + l, y + w)]
            for j, (it2, x2, y2, z2, l2, w2, h2) in enumerate(boxes):
                if i == j:
                    continue
                dx = min(x + l, x2 + l2) - max(x, x2)
                dy = min(y + w, y2 + w2) - max(y, y2)
                dz = min(z + h, z2 + h2) - max(z, z2)
                if j > i and dx > 0 and dy > 0 and dz > 0:
                    errors.append(f"ULD {u}: items {it} and {it2} intersect")
                if z2 + h2 == z:
                    fragile = items[it2][7] == 1
                    if dx > 0 and dy > 0:
                        if fragile:
                            errors.append(f"ULD {u}: item {it} on the fragile item {it2}")
                        area += dx * dy
                    if not fragile:
                        for k, (px, py) in enumerate(pts):
                            if x2 <= px <= x2 + l2 and y2 <= py <= y2 + w2:
                                corners[k] = True
            if z > 0:
                if support == 'corners' and not all(corners):
                    errors.append(f"ULD {u}: item {it} not supported (corners)")
                if support == 'full' and area < l * w:
                    errors.append(f"ULD {u}: item {it} not supported (base)")

        if weight > U['cap'] + 1e-6:
            errors.append(f"ULD {u}: weight {weight} > {U['cap']}")
        if weight > 0:
            cx, cy = mx / weight, my / weight
            if abs(cx - U['L'] / 2) > U['dev'][0] + 1e-6 or abs(cy - U['W'] / 2) > U['dev'][1] + 1e-6:
                n_cog += 1
            if mz / weight > U['dev'][2] + 1e-6:
                n_cog_z += 1
        util.append(vol / U['vol'])

    for e in errors[:20]:
        print("ERROR:", e)
    print(f"feasible {int(not errors)} n_ulds {len(loads)} cost {cost:.1f} cog_violations {n_cog} "
          f"cog_z_violations {n_cog_z} utilizations {' '.join(f'{100 * v:.2f}' for v in util)}")
    sys.exit(1 if errors else 0)


if __name__ == '__main__':
    main()
