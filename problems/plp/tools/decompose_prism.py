"""
Descomposicion convexa exacta de piezas prismaticas (extrusion de un poligono 2D a lo largo de un eje).

Uso: decompose_prism.py benchs pieza1.stl [pieza2.stl ...]
Escribe benchs/decompositions/<pieza sin extension>.obj con un grupo (o) por parte convexa.

El poligono de la base (las caras en el nivel inferior del eje del prisma) se triangula (ear clipping) y los
triangulos se fusionan en poligonos convexos (Hertel-Mehlhorn: se elimina una diagonal si el resultado es
convexo); cada poligono convexo se extruye. Un marco (rectangulo con un hueco rectangular, alineados a los ejes)
se divide en cuatro cajas. Las partes no se solapan y su union es la pieza: se verifica que la suma de sus
volumenes sea el volumen de la malla.
"""
import sys, os
import numpy as np
import trimesh


def cross2(o, a, b):
    return (a[0]-o[0])*(b[1]-o[1]) - (a[1]-o[1])*(b[0]-o[0])


def area2(P):
    return sum(P[i][0]*P[(i+1) % len(P)][1] - P[(i+1) % len(P)][0]*P[i][1] for i in range(len(P)))


def loops_from_edges(edges):
    """ciclos simples a partir de aristas de borde (pares de indices); un vertice donde se tocan dos lazos
    (vertice pellizcado) separa los ciclos"""
    adj = {}
    for k, (a, b) in enumerate(edges):
        adj.setdefault(a, []).append((b, k))
        adj.setdefault(b, []).append((a, k))
    used = [False]*len(edges)
    loops = []
    for s in adj:
        path = [s]
        while True:
            cur = path[-1]
            nxt = next(((v, k) for v, k in adj[cur] if not used[k]), None)
            if nxt is None:
                break
            used[nxt[1]] = True
            v = nxt[0]
            if v in path:
                i = path.index(v)
                loops.append(path[i:])
                path = path[:i+1]
            else:
                path.append(v)
    return loops


def remove_collinear(P, idx):
    out = []
    n = len(idx)
    for i in range(n):
        a, b, c = P[idx[i-1]], P[idx[i]], P[idx[(i+1) % n]]
        if abs(cross2(a, b, c)) > 1e-12:
            out.append(idx[i])
    return out


def ear_clip(P, idx):
    """triangulacion de un poligono simple en sentido antihorario"""
    idx = list(idx)
    tris = []
    while len(idx) > 3:
        n = len(idx)
        for i in range(n):
            a, b, c = idx[i-1], idx[i], idx[(i+1) % n]
            if cross2(P[a], P[b], P[c]) <= 1e-12:
                continue
            inside = False
            for j in idx:
                if j in (a, b, c) or P[j] in (P[a], P[b], P[c]):
                    continue
                if cross2(P[a], P[b], P[j]) >= -1e-12 and cross2(P[b], P[c], P[j]) >= -1e-12 and cross2(P[c], P[a], P[j]) >= -1e-12:
                    inside = True
                    break
            if not inside:
                tris.append([a, b, c])
                idx.pop(i)
                break
        else:
            raise RuntimeError("ear clipping: no se encontro oreja")
    tris.append(idx)
    return tris


def is_convex(P, poly):
    n = len(poly)
    return all(cross2(P[poly[i-1]], P[poly[i]], P[poly[(i+1) % n]]) >= -1e-12 for i in range(n))


def hertel_mehlhorn(P, polys):
    changed = True
    while changed:
        changed = False
        for x in range(len(polys)):
            for y in range(x+1, len(polys)):
                p, q = polys[x], polys[y]
                # arista compartida: (a,b) en p y (b,a) en q
                for k in range(len(p)):
                    a, b = p[k], p[(k+1) % len(p)]
                    for l in range(len(q)):
                        if q[l] == b and q[(l+1) % len(q)] == a:
                            i = p.index(b)
                            pp = p[i:] + p[:i]          # empieza en b, termina en a
                            j = q.index(a)
                            qq = q[j:] + q[:j]          # empieza en a, termina en b
                            m = pp + qq[1:-1]
                            if is_convex(P, m):
                                polys[x] = m
                                polys.pop(y)
                                changed = True
                            break
                    if changed:
                        break
                if changed:
                    break
            if changed:
                break
    return polys


def decompose(mesh):
    V = mesh.vertices
    for axis in range(3):
        levels = np.unique(np.round(V[:, axis], 9))
        if len(levels) == 2:
            break
    else:
        raise RuntimeError("no es un prisma alineado a un eje")
    lo, hi = levels
    other = [c for c in range(3) if c != axis]
    cap = [f for f in mesh.faces if all(abs(V[v, axis]-lo) < 1e-9 for v in f)]
    count = {}
    for f in cap:
        for k in range(3):
            e = tuple(sorted((f[k], f[(k+1) % 3])))
            count[e] = count.get(e, 0) + 1
    loops = loops_from_edges([e for e, c in count.items() if c == 1])
    P = {v: (V[v, other[0]], V[v, other[1]]) for v in range(len(V))}
    polys2d = []
    def inside(poly, q):
        c = False
        for k in range(len(poly)):
            (x1, y1), (x2, y2) = P[poly[k]], P[poly[(k+1) % len(poly)]]
            if (y1 > q[1]) != (y2 > q[1]) and q[0] < x1 + (q[1]-y1)*(x2-x1)/(y2-y1):
                c = not c
        return c
    def contains(outer, inner):
        pts = [P[i] for i in inner if i not in outer]
        return len(pts) > 0 and all(inside(outer, q) for q in pts)
    framed = len(loops) == 2 and (contains(loops[0], loops[1]) or contains(loops[1], loops[0]))
    if framed and set(loops[0]) & set(loops[1]):
        #hueco que toca el borde exterior en un vertice: un solo poligono (debilmente simple) que recorre el
        #borde exterior (antihorario) y luego el hueco (horario) desde el vertice compartido
        outer, hole = (loops[0], loops[1]) if contains(loops[0], loops[1]) else (loops[1], loops[0])
        if area2([P[i] for i in outer]) < 0:
            outer = outer[::-1]
        if area2([P[i] for i in hole]) > 0:
            hole = hole[::-1]
        v = next(i for i in outer if i in hole)
        k = outer.index(v); outer = outer[k:] + outer[:k]
        k = hole.index(v); hole = hole[k:] + hole[:k]
        dup = max(P) + 1          #copia del vertice compartido (indices distintos en el poligono)
        P[dup] = P[v]
        loops = [outer + [dup] + hole[1:]]
        framed = False
    if not framed:
        #uno o mas poligonos simples (pueden tocarse en un vertice)
        for idx in loops:
            if area2([P[i] for i in idx]) < 0:
                idx = idx[::-1]
            idx = remove_collinear(P, idx)
            for poly in hertel_mehlhorn(P, ear_clip(P, idx)):
                polys2d.append([P[i] for i in poly])
    elif len(loops) == 2:
        # marco: rectangulo exterior con hueco rectangular
        boxes = [np.array([P[i] for i in l]) for l in loops]
        boxes.sort(key=lambda b: -(np.ptp(b[:, 0])*np.ptp(b[:, 1])))
        (X0, Y0), (X1, Y1) = boxes[0].min(0), boxes[0].max(0)
        (x0, y0), (x1, y1) = boxes[1].min(0), boxes[1].max(0)
        for b in boxes:
            if len(b) != 4 or abs(abs(area2(list(map(tuple, b))))/2 - np.ptp(b[:, 0])*np.ptp(b[:, 1])) > 1e-9:
                raise RuntimeError("hueco no rectangular")
        rects = [(X0, Y0, X1, y0), (X0, y1, X1, Y1), (X0, y0, x0, y1), (x1, y0, X1, y1)]
        for a, b, c, d in rects:
            if c > a and d > b:
                polys2d.append([(a, b), (c, b), (c, d), (a, d)])
    else:
        raise RuntimeError("base con %d lazos" % len(loops))
    parts = []
    for poly in polys2d:
        pts = []
        for h in (lo, hi):
            for (u, v) in poly:
                p = [0.0, 0.0, 0.0]
                p[axis], p[other[0]], p[other[1]] = h, u, v
                pts.append(p)
        parts.append((np.array(pts), abs(area2(poly))/2*(hi-lo)))
    return parts


def main():
    root = sys.argv[1]
    for path in sys.argv[2:]:
        mesh = trimesh.load(os.path.join(root, path), force='mesh', process=True)
        parts = decompose(mesh)
        vol = sum(v for _, v in parts)
        print("%s: %d partes, volumen partes %.6f, malla %.6f" % (path, len(parts), vol, abs(mesh.volume)))
        if abs(vol - abs(mesh.volume)) > 1e-6*max(1.0, vol):
            raise RuntimeError("el volumen de las partes no coincide con el de la malla")
        out = os.path.join(root, "decompositions", os.path.splitext(path)[0] + ".obj")
        os.makedirs(os.path.dirname(out), exist_ok=True)
        with open(out, "w") as f:
            f.write('# Descomposicion convexa exacta de "%s" (prisma; tools/decompose_prism.py)\n' % path)
            base = 1
            for k, (pts, _) in enumerate(parts):
                hull = trimesh.convex.convex_hull(pts)
                f.write("o part%d\n" % (k+1))
                for p in hull.vertices:
                    f.write("v %.17g %.17g %.17g\n" % tuple(p))
                for fc in hull.faces:
                    f.write("f %d %d %d\n" % tuple(int(i)+base for i in fc))
                base += len(hull.vertices)


if __name__ == "__main__":
    main()
