"""
Descomposicion convexa conservadora de mallas generales (la union de las partes contiene a la pieza).

uso: decompose_mesh.py benchs [--threshold 0.1] [--kdop 26] [--dec carpeta] pieza1 [pieza2 ...]
Escribe benchs/<carpeta>/<pieza sin extension>.obj (por defecto benchs/decompositions) con un grupo (o) por parte.

1. CoACD (Wei et al. 2022) corta la malla con planos y toma la envolvente convexa de cada trozo. --threshold
   controla la concavidad admitida (mayor: menos partes, mas gruesas). Entre partes vecinas quedan huecos muy
   pequenos (hasta ~6e-5 de la diagonal en las piezas del ajedrez); --offset desplaza las caras de cada parte hacia
   afuera al menos --offset (por defecto 2e-4 de la diagonal) para cubrirlos: escalando la parte desde su centroide
   (--cover scale, conserva vertices y caras) o desplazando cada cara (--cover offset, mas ajustado).
2. --merge a: en cada parte, las caras con normales a menos de a grados se fusionan en una (normal media,
   desplazada hasta el vertice mas alejado); la parte resultante contiene a la original y tiene muchas menos caras
   (las caras casi coplanares de CoACD generan NFP con miles de facetas).
3. --kdop k (0: no): cada envolvente se reemplaza por su k-DOP, la interseccion de los semiespacios
   d.x <= max_v d.v para k direcciones fijas (6: caja; 14: + diagonales de vertices; 26: + diagonales de aristas),
   si tiene menos caras. El k-DOP contiene a la envolvente.
Se informa el volumen de la union (por muestreo) respecto del de la malla y se verifica, con puntos interiores de la
malla, que todos queden dentro de la union.
requiere: numpy, scipy, trimesh, coacd
"""
import sys, os, argparse
import numpy as np
import trimesh
from scipy.spatial import ConvexHull, HalfspaceIntersection


def kdop_dirs(k):
    d = [(1, 0, 0), (0, 1, 0), (0, 0, 1)]
    if k >= 14: d += [(1, 1, 1), (1, 1, -1), (1, -1, 1), (-1, 1, 1)]
    if k >= 26: d += [(1, 1, 0), (1, -1, 0), (1, 0, 1), (1, 0, -1), (0, 1, 1), (0, 1, -1)]
    d = np.array(d, float)
    d /= np.linalg.norm(d, axis=1)[:, None]
    return np.vstack([d, -d])


def kdop(P, k, m=0):
    """k-DOP de P, con las normales de las m caras de mayor area de su envolvente como direcciones adicionales"""
    D = kdop_dirs(k)
    if m:
        h = ConvexHull(P)
        tri = P[h.simplices]
        area = np.linalg.norm(np.cross(tri[:, 1]-tri[:, 0], tri[:, 2]-tri[:, 0]), axis=1)
        D = np.vstack([D, h.equations[np.argsort(-area)[:m], :3]])
    h = np.max(P @ D.T, axis=0)
    hs = np.hstack([D, -h[:, None]])      # D.x - h <= 0
    c = P.mean(axis=0)
    V = HalfspaceIntersection(hs, c).intersections
    #envolvente junto con P: la contiene aunque la interseccion de semiespacios tenga errores numericos
    Q = np.vstack([V, P])
    return Q[ConvexHull(Q).vertices]


def offset(P, delta):
    """politopo con las caras de conv(P) desplazadas delta hacia afuera (contiene a conv(P))"""
    E = ConvexHull(P).equations.copy()
    E[:, 3] -= delta
    V = HalfspaceIntersection(E, P.mean(axis=0)).intersections
    V = V[ConvexHull(V).vertices]
    #comprobacion: los vertices de P dentro de conv(V) (si falla por errores numericos, se agrega P)
    F = ConvexHull(V).equations
    if (P @ F[:, :3].T + F[:, 3]).max() > 0:
        Q = np.vstack([V, P]); V = Q[ConvexHull(Q).vertices]
    return V


def scale_cover(P, delta):
    """escala conv(P) desde su centroide para que cada cara se aleje al menos delta (mismos vertices y caras)"""
    h = ConvexHull(P)
    c = P[h.vertices].mean(axis=0)
    dist = -(h.equations[:, :3] @ c + h.equations[:, 3])      # distancia del centroide a cada cara (> 0)
    s = 1 + delta/max(dist.min(), 1e-12)
    return c + s*(P[h.vertices]-c)


def merge_faces(P, angle):
    """politopo que contiene a conv(P), con las caras de normales a menos de `angle` grados fusionadas: cada grupo
    usa su normal media (ponderada por area) desplazada hasta el vertice mas alejado"""
    h = ConvexHull(P)
    tri = P[h.simplices]
    area = np.linalg.norm(np.cross(tri[:, 1]-tri[:, 0], tri[:, 2]-tri[:, 0]), axis=1)
    N = h.equations[:, :3]
    order = np.argsort(-area)
    cos = np.cos(np.radians(angle))
    groups = []                              # (normal acumulada, area)
    for k in order:
        for g in groups:
            if np.dot(g[0]/np.linalg.norm(g[0]), N[k]) >= cos:
                g[0] += area[k]*N[k]; g[1] += area[k]; break
        else:
            groups.append([area[k]*N[k].copy(), area[k]])
    D = np.array([g[0]/np.linalg.norm(g[0]) for g in groups])
    hgt = np.max(P @ D.T, axis=0)
    V = HalfspaceIntersection(np.hstack([D, -hgt[:, None]]), P[h.vertices].mean(axis=0)).intersections
    V = V[ConvexHull(V).vertices]
    F = ConvexHull(V).equations
    if (P @ F[:, :3].T + F[:, 3]).max() > 1e-9:
        Q = np.vstack([V, P]); V = Q[ConvexHull(Q).vertices]
    return V


def union_volume(parts, n=200000, seed=0):
    allv = np.vstack(parts); lo, hi = allv.min(0), allv.max(0)
    pts = np.random.default_rng(seed).uniform(lo, hi, (n, 3))
    inside = np.zeros(n, bool)
    for P in parts:
        E = ConvexHull(P).equations
        inside |= np.all(pts @ E[:, :3].T + E[:, 3] <= 1e-9, axis=1)
    return inside.mean()*np.prod(hi-lo)


def contained(mesh, parts, n=40000, seed=1):
    """fraccion de puntos interiores de la malla fuera de la union (debe ser 0)"""
    pts = trimesh.sample.volume_mesh(mesh, n)
    out = np.ones(len(pts), bool)
    for P in parts:
        E = ConvexHull(P).equations
        out &= ~np.all(pts @ E[:, :3].T + E[:, 3] <= 1e-7, axis=1)
    return out.mean(), len(pts)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('root'); ap.add_argument('pieces', nargs='+')
    ap.add_argument('--threshold', type=float, default=0.1)
    ap.add_argument('--kdop', type=int, default=26)
    ap.add_argument('--offset', type=float, default=2e-4, help='desplazamiento de las caras hacia afuera, como fraccion de la diagonal de la pieza (cubre los huecos de CoACD entre partes)')
    ap.add_argument('--merge', type=float, default=3.0, help='fusiona las caras de cada parte con normales a menos de este angulo (grados; 0: no); la parte resultante contiene a la original')
    ap.add_argument('--cover', choices=['scale', 'offset'], default='scale', help='como se agranda cada parte: escalado desde el centroide (mismos vertices) o desplazamiento de caras (mas ajustado, mas vertices)')
    ap.add_argument('--hullfaces', type=int, default=0, help='k-DOP: normales adicionales de las m caras mas grandes de la envolvente')
    ap.add_argument('--dec', default='decompositions', help='carpeta de salida bajo benchs (para guardar varias resoluciones)')
    a = ap.parse_args()
    import coacd
    coacd.set_log_level('error')
    for path in a.pieces:
        mesh = trimesh.load(os.path.join(a.root, path), force='mesh')
        res = coacd.run_coacd(coacd.Mesh(mesh.vertices, mesh.faces), threshold=a.threshold, preprocess_mode='off')
        parts = [np.asarray(p[0], float) for p in res]
        if a.merge > 0:
            parts = [merge_faces(P, a.merge) for P in parts]
        if a.offset > 0:
            diag = np.linalg.norm(mesh.bounds[1]-mesh.bounds[0])
            parts = [offset(P, a.offset*diag) if a.cover == 'offset' else scale_cover(P, a.offset*diag) for P in parts]
        if a.kdop:
            parts = [Q if len(ConvexHull(Q).simplices) < len(ConvexHull(P).simplices) else P for P, Q in ((P, kdop(P, a.kdop, a.hullfaces)) for P in parts)]
        nf = [len(ConvexHull(P).simplices) for P in parts]
        uv = union_volume(parts)
        frac, ns = contained(mesh, parts)
        print('%s: %d partes, caras %d-%d, union %.4g (+%.1f%%), puntos interiores fuera de la union: %d de %d'
              % (path, len(parts), min(nf), max(nf), uv, 100*(uv/mesh.volume-1), int(round(frac*ns)), ns))
        out = os.path.join(a.root, a.dec, os.path.splitext(path)[0] + '.obj')
        os.makedirs(os.path.dirname(out), exist_ok=True)
        with open(out, 'w') as f:
            f.write('# Descomposicion convexa conservadora de "%s" (CoACD threshold %g, offset %g, k-DOP %d + %d caras; tools/decompose_mesh.py)\n'
                    % (path, a.threshold, a.offset, a.kdop, a.hullfaces))
            base = 1
            for k, P in enumerate(parts):
                h = ConvexHull(P)
                idx = sorted(set(h.vertices)); remap = {v: i for i, v in enumerate(idx)}
                f.write('o part%d\n' % (k+1))
                for v in idx: f.write('v %.17g %.17g %.17g\n' % tuple(P[v]))
                for s in h.simplices:
                    # orientacion hacia afuera
                    a0, b0, c0 = P[s[0]], P[s[1]], P[s[2]]
                    nrm = np.cross(b0-a0, c0-a0)
                    if np.dot(nrm, a0 - P[idx].mean(axis=0)) < 0: s = s[[0, 2, 1]]
                    f.write('f %d %d %d\n' % tuple(remap[v]+base for v in s))
                base += len(idx)


if __name__ == '__main__':
    main()
