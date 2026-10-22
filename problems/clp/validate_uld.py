#!/usr/bin/env python3
"""
Validador independiente de soluciones de BSG_ULD (instancias de Paquay et al. 2018).

uso: validate_uld.py <instancia.csv> <uld.csv> <solucion.csv> [--support corners|full|none]
       [--edge e --edge_offset d] [--min_overlap o --padding p] [--cog_rel r] [--allow_unloaded]

Verifica: cada caja cargada exactamente una vez (o a lo mas una vez, con --allow_unloaded);
orientacion permitida; caja dentro del ULD y de sus recortes; bajo la altura d, fuera del borde
de ancho e del piso (con subestructura, nada bajo d y el piso dentro del borde a la altura d);
sin intersecciones; peso maximo; soporte (las cuatro esquinas inferiores de cada caja sobre
cajas no fragiles o el piso o, si o>0, parte de la base apoyada y al menos o de la base apoyada
directamente o sobre relleno de altura a lo mas p sobre cajas no fragiles; o la base completa
con --support full); nada sobre cajas fragiles. Cuenta los ULDs con el centro de gravedad fuera de la desviacion permitida
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
    opts = {'--support': 'corners', '--edge': '0', '--edge_offset': '0', '--min_overlap': '0',
            '--padding': '0', '--cog_rel': '0'}
    for k in list(opts):
        if k in args:
            i = args.index(k)
            opts[k] = args[i + 1]
            del args[i:i + 2]
    allow_unloaded = '--allow_unloaded' in args
    if allow_unloaded:
        args.remove('--allow_unloaded')
    support = opts['--support']
    edge, delta = float(opts['--edge']), float(opts['--edge_offset'])
    min_overlap, padding = float(opts['--min_overlap']), float(opts['--padding'])
    cog_rel = float(opts['--cog_rel'])
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
        dev = f[5:8]
        if cog_rel > 0:
            dev = [cog_rel * L / 2, cog_rel * W / 2, f[7]]
        ulds[int(f[21])] = dict(L=L, W=W, H=H, cap=f[3], cost=f[4], dev=dev, cuts=cuts, vol=f[20])

    sol = read_csv(solf)[1:]
    errors = []
    used = {}
    loads = {}
    sub = {}
    for r in sol:
        u, ut, sb, it = int(r[0]), int(r[1]), int(r[2]), int(r[3])
        x, y, z, l, w, h = map(float, r[4:10])
        if it in used:
            errors.append(f"item {it} loaded twice")
        used[it] = u
        sub[u] = sb
        loads.setdefault(u, (ut, []))[1].append((it, x, y, z, l, w, h))

    unloaded = [it for it in range(len(items)) if it not in used]
    if unloaded and not allow_unloaded:
        errors.append(f"items not loaded: {unloaded[:10]}")

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
            # borde del piso
            if z < delta and (sub[u] or x < edge or y < edge or x + l > U['L'] - edge or y + w > U['W'] - edge):
                errors.append(f"ULD {u}: item {it} on the edge of the ULD")
            # recortes (el ULD es convexo: basta revisar las esquinas en el plano x-z)
            for (a, b, c, sign) in U['cuts']:
                for (px, pz) in [(x, z), (x + l, z), (x, z + h), (x + l, z + h)]:
                    if sign * (a * pz + b * px) > sign * c + 1e-6:
                        errors.append(f"ULD {u}: item {it} outside the cut ({a},{b},{c})")
                        break

        # intersecciones, soporte y cajas fragiles
        for i, (it, x, y, z, l, w, h) in enumerate(boxes):
            corners = [False] * 4
            area = 0.0      # apoyo directo
            indirect = 0.0  # apoyo sobre relleno
            pts = [(x, y), (x + l, y), (x, y + w), (x + l, y + w)]
            # caras de apoyo: (x1, y1, x2, y2, altura, fragil); el piso elevado de la subestructura
            faces = []
            if sub[u] and z == delta:
                faces.append((edge, edge, U['L'] - edge, U['W'] - edge, z, False))
            for j, (it2, x2, y2, z2, l2, w2, h2) in enumerate(boxes):
                if i == j:
                    continue
                dx = min(x + l, x2 + l2) - max(x, x2)
                dy = min(y + w, y2 + w2) - max(y, y2)
                dz = min(z + h, z2 + h2) - max(z, z2)
                if j > i and dx > 0 and dy > 0 and dz > 0:
                    errors.append(f"ULD {u}: items {it} and {it2} intersect")
                if z - padding <= z2 + h2 <= z:
                    faces.append((x2, y2, x2 + l2, y2 + w2, z2 + h2, items[it2][7] == 1))
            for (fx1, fy1, fx2, fy2, fz, fragile) in faces:
                dx = min(x + l, fx2) - max(x, fx1)
                dy = min(y + w, fy2) - max(y, fy1)
                ov = dx > 0 and dy > 0
                if fz == z:
                    if ov:
                        if fragile:
                            errors.append(f"ULD {u}: item {it} on a fragile item")
                        else:
                            area += dx * dy
                    if not fragile:
                        for k, (px, py) in enumerate(pts):
                            if fx1 <= px <= fx2 and fy1 <= py <= fy2:
                                corners[k] = True
                elif ov and not fragile:
                    indirect += dx * dy
            if z > 0:
                if support == 'corners' and not all(corners) and not (
                        min_overlap > 0 and area > 0 and area + indirect >= min_overlap * l * w):
                    errors.append(f"ULD {u}: item {it} not supported")
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
    print(f"feasible {int(not errors)} n_ulds {len(loads)} unloaded {len(unloaded)} cost {cost:.1f} cog_violations {n_cog} "
          f"cog_z_violations {n_cog_z} utilizations {' '.join(f'{100 * v:.2f}' for v in util)}")
    sys.exit(1 if errors else 0)


if __name__ == '__main__':
    main()
