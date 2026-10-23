#!/usr/bin/env python3
"""
Certificado exacto de una solucion (formato ESICUP), sin tolerancias.

Los vertices de las partes (archivos de la instancia o benchs/decompositions, como check_solution.py) y las
traslaciones de la solucion son doubles, es decir, racionales exactos: todo el calculo se hace con
fractions.Fraction. Se verifica:
* contencion: 0 <= x <= W, 0 <= y <= D, z >= 0 para todos los vertices;
* no solapamiento: para cada par de partes (convexas) de piezas distintas se busca un eje separador
  (normales de las caras de cada parte y productos cruz de sus aristas, calculados en racionales) con
  max_A n.a <= min_B n.b (las piezas pueden tocarse). Los ejes se prueban en el orden de su separacion en punto
  flotante.
Si todo se cumple, la solucion es factible (con la altura exacta informada). Si no, se informa la violacion exacta
(distancia fuera del contenedor o, para un par sin eje separador, el menor solapamiento sobre los ejes probados):
en piezas que se tocan exactamente (p. ej., una fila que llena el ancho del contenedor) las posiciones de contacto
pueden no ser representables en doubles, y queda una violacion del orden del redondeo.

uso: certify_solution.py instancia.json solucion.json carpeta_benchs
requiere: numpy, scipy
"""
import json, sys, os
from fractions import Fraction
import numpy as np
from scipy.spatial import ConvexHull

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

inst_file, sol_file, root = sys.argv[1], sys.argv[2], sys.argv[3]
I = json.load(open(inst_file)); S = json.load(open(sol_file))
W, D = Fraction(I['container']['size-x']), Fraction(I['container']['size-y'])


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


def read_stl_vertices(path):
    data = open(path, 'rb').read()
    n = int.from_bytes(data[80:84], 'little') if len(data) >= 84 else -1
    if n >= 0 and len(data) == 84 + 50*n:
        rec = np.frombuffer(data[84:], dtype=np.dtype([('n', '<f4', 3), ('v', '<f4', (3, 3)), ('a', '<u2')]), count=n)
        return rec['v'].reshape(-1, 3).astype(float)
    return np.array([[float(x) for x in l.split()[1:4]] for l in data.decode().splitlines() if l.strip().startswith('vertex')])


def shells(F):
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
    elif rel.lower().endswith('.stl'):
        parts = [read_stl_vertices(root + '/' + rel)]
    else:
        parts = [read_obj(root + '/' + rel)[0]]
    out = []
    for P in parts:
        h = ConvexHull(P)
        idx = sorted(set(h.vertices))
        remap = {v: k for k, v in enumerate(idx)}
        tris = [[remap[v] for v in s] for s in h.simplices]
        edges = sorted({tuple(sorted((t[a], t[(a+1) % 3]))) for t in tris for a in range(3)})
        out.append((P[idx], tris, edges))
    #alineacion al minimo (itemOrigin = AlignToMinimum), en racionales
    lo = [min(Fraction(float(x)) for P, _, _ in out for x in P[:, c]) for c in range(3)]
    return [([tuple(Fraction(float(v[c])) - lo[c] for c in range(3)) for v in P], tris, edges) for P, tris, edges in out]


def sub(a, b): return (a[0]-b[0], a[1]-b[1], a[2]-b[2])
def cross(a, b): return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])
def dot(a, b): return a[0]*b[0] + a[1]*b[1] + a[2]*b[2]


pieces = {}
for it in I['item-types']:
    pieces[it['path'].split('/')[-1]] = load_parts(it['path'])

# partes ubicadas: vertices en racionales (la pieza alineada al minimo, mas la traslacion)
placed = []   # (item, vertices Fraction, vertices float, tris, edges)
for k, it in enumerate(S['items']):
    M = it['transformation']['matrix']
    R = [[M[r][c] for c in range(3)] for r in range(3)]
    if any(R[r][c] != (1.0 if r == c else 0.0) for r in range(3) for c in range(3)):
        sys.exit('solo traslaciones')
    T = [Fraction(M[3][c]) for c in range(3)]
    for P, tris, edges in pieces[it['name']]:
        Vq = [tuple(x + T[c] for c, x in enumerate(v)) for v in P]
        Vf = np.array([[float(x) for x in v] for v in Vq])
        placed.append((k, Vq, Vf, tris, edges))

# contencion y altura exactas
height = Fraction(0)
out_items, out_max = set(), Fraction(0)
for k, Vq, _, _, _ in placed:
    for v in Vq:
        e = max(-v[0], v[0]-W, -v[1], v[1]-D, -v[2])
        if e > 0: out_items.add(k); out_max = max(out_max, e)
        height = max(height, v[2])

# candidatos de ejes (en punto flotante, para ordenarlos) y prueba exacta
def axes(A, B):
    out = []
    for (Vq, Vf, tris, edges) in (A, B):
        for t in tris:
            out.append(('f', Vq, t))
    for ea in A[3]:
        for eb in B[3]:
            out.append(('e', ea, eb))
    return out


def axis_exact(c, A, B):
    if c[0] == 'f':
        Vq, t = c[1], c[2]
        return cross(sub(Vq[t[1]], Vq[t[0]]), sub(Vq[t[2]], Vq[t[0]]))
    ea, eb = c[1], c[2]
    return cross(sub(A[0][ea[1]], A[0][ea[0]]), sub(B[0][eb[1]], B[0][eb[0]]))


def axis_float(c, A, B):
    if c[0] == 'f':
        Vq, t = c[1], c[2]
        V = np.array([[float(x) for x in Vq[i]] for i in t])
        return np.cross(V[1]-V[0], V[2]-V[0])
    ea, eb = c[1], c[2]
    return np.cross(A[1][ea[1]]-A[1][ea[0]], B[1][eb[1]]-B[1][eb[0]])


pairs = 0
bad = []
for x in range(len(placed)):
    kx, Ax, Afx, trx, edx = placed[x]
    for y in range(x+1, len(placed)):
        ky, Ay, Afy, try_, edy = placed[y]
        if kx == ky: continue
        # cajas (exactas)
        sep = False
        for c in range(3):
            if max(v[c] for v in Ax) <= min(v[c] for v in Ay) or max(v[c] for v in Ay) <= min(v[c] for v in Ax):
                sep = True; break
        if sep: continue
        pairs += 1
        A = (Ax, Afx, trx, edx); B = (Ay, Afy, try_, edy)
        cands = []
        for c in axes((Ax, Afx, trx, edx), (Ay, Afy, try_, edy)):
            n = axis_float(c, A, B)
            nn = np.linalg.norm(n)
            if nn < 1e-12: continue
            n = n/nn
            pa, pb = Afx @ n, Afy @ n
            cands.append((max(pb.min()-pa.max(), pa.min()-pb.max()), c))
        cands.sort(key=lambda z: -z[0])
        ok = False
        for g, c in cands[:20]:
            n = axis_exact(c, A, B)
            if n == (0, 0, 0): continue
            pa = [dot(n, v) for v in Ax]; pb = [dot(n, v) for v in Ay]
            if max(pa) <= min(pb) or max(pb) <= min(pa):
                ok = True; break
        if not ok:
            #violacion exacta: el menor solapamiento (normalizado) sobre los ejes probados
            best = None
            for g, c in cands[:20]:
                n = axis_exact(c, A, B)
                if n == (0, 0, 0): continue
                pa = [dot(n, v) for v in Ax]; pb = [dot(n, v) for v in Ay]
                ov = min(max(pa)-min(pb), max(pb)-min(pa)) / float(dot(n, n))**0.5
                best = ov if best is None else min(best, ov)
            bad.append((kx, ky, float(best)))

print('pares de partes cercanas:', pairs)
print('pares sin eje separador exacto:', len(bad), ('(max solapamiento %.3g)' % max(b[2] for b in bad)) if bad else '')
print('items fuera del contenedor:', len(out_items), ('(max %.3g)' % float(out_max)) if out_items else '')
print('height: %.17g' % float(height))
print('certified:', 'yes' if not bad and not out_items else 'no')
