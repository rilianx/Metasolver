"""
Diagramas 2D del metodo (requiere numpy, scipy, matplotlib).

Reimplementa en 2D (ejes x y z) los mismos pasos de BSG_PLP sobre un ejemplo pequeno, y dibuja lo que hacen:
  figures/construction.pdf   construccion greedy paso a paso (descenso por LP con facetas de NFP) y compactacion
                             global por LP
  figures/nfp.pdf            NFP de dos partes (deslizamiento, suma de Minkowski) y penetracion
  figures/axis.pdf           Proposicion 1: penetracion a lo largo de un eje (cero fuera de un intervalo, concava dentro)
  figures/descend.pdf        un descenso paso a paso: region de confianza, facetas elegidas y solucion de cada LP
  figures/passes.pdf         todas las pasadas de MinimizeOverlap de overlap.pdf (piezas movidas y pesos GLS)
  figures/overlap.pdf        movimiento exacto a lo largo de un eje (funcion phi concava por
                             tramos); minimizacion del solapamiento con pesos GLS hasta separar

uso (desde problems/plp/paper): python make_diagrams.py
"""
import os
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon as MplPolygon, FancyArrowPatch
from scipy.spatial import ConvexHull
from scipy.optimize import linprog

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, 'figures')
INK, MUTED, GRID = '#2b2b2a', '#8a8984', '#e4e3df'
ACCENT, ACCENT2, RED = '#2a78d6', '#eb6834', '#c8372d'
plt.rcParams.update({'font.size': 8, 'font.family': 'serif', 'axes.edgecolor': MUTED, 'axes.labelcolor': INK,
                     'xtick.color': INK, 'ytick.color': INK, 'axes.spines.top': False, 'axes.spines.right': False})
COLORS = ['#8fb8e8', '#f2b58c', '#a8d5a2', '#d8b4e2', '#f5d58a', '#9fd6d2', '#e8a3a8', '#c2c0b6', '#b9cde8',
          '#f0c9a8', '#c6e2b5', '#dcc6ea']
TOL = 1e-9


# ---------------- geometria ----------------
class Piece:
    """union de partes convexas, en su marco local (minimo de la caja = origen)"""
    def __init__(self, parts):
        parts = [np.asarray(p, float) for p in parts]
        lo = np.min([p.min(0) for p in parts], 0)
        self.parts = [p[ConvexHull(p).vertices] - lo for p in parts]
        allv = np.vstack(self.parts)
        self.w, self.h = allv.max(0)
        self.area = sum(ConvexHull(p).volume for p in self.parts)


def nfp(a, b):
    """NFP de la parte a (movil) respecto de b: conv{vb - va}; H-representacion n.r <= d (n unitario)"""
    pts = (b[None, :, :] - a[:, None, :]).reshape(-1, 2)
    h = ConvexHull(pts)
    return pts[h.vertices], h.equations[:, :2], -h.equations[:, 2]


NFP = {}
def nfp_c(pi, ai, pj, bj, pieces):
    k = (pi, ai, pj, bj)
    if k not in NFP: NFP[k] = nfp(pieces[pi].parts[ai], pieces[pj].parts[bj])
    return NFP[k]


def penetration(N, r):
    _, n, d = N
    return max(0.0, np.min(d - n @ r))


def pair_pen(i, j, ty, t, pieces):
    s = 0.0
    for a in range(len(pieces[ty[i]].parts)):
        for b in range(len(pieces[ty[j]].parts)):
            s += penetration(nfp_c(ty[i], a, ty[j], b, pieces), t[i] - t[j])
    return s


def placed_polys(ty, t, pieces, idx=None):
    idx = range(len(ty)) if idx is None else idx
    return [(i, [p + t[i] for p in pieces[ty[i]].parts]) for i in idx]


def cross2(a, b):
    return a[..., 0]*b[..., 1] - a[..., 1]*b[..., 0]


def clip(subject, clipper):
    """interseccion de dos poligonos convexos (Sutherland-Hodgman); para sombrear solapamientos"""
    out = list(subject)
    c = clipper if cross2(clipper[1]-clipper[0], clipper[2]-clipper[1]) > 0 else clipper[::-1]
    for k in range(len(c)):
        a, b = c[k], c[(k+1) % len(c)]
        inp, out = out, []
        if not inp: break
        side = lambda p: cross2(b-a, p-a)
        for m in range(len(inp)):
            p, q = inp[m], inp[(m+1) % len(inp)]
            if side(q) >= 0:
                if side(p) < 0: out.append(p + (q-p)*side(p)/(side(p)-side(q)))
                out.append(q)
            elif side(p) >= 0:
                out.append(p + (q-p)*side(p)/(side(p)-side(q)))
    return np.array(out) if len(out) >= 3 else None


# ---------------- descenso por LP (construccion y mejora local) ----------------
def descend(i, ty, t, placed, pieces, W, slide=0.065, rounds=50):
    """la pieza i baja desde t[i]: LP en (x, z) con una faceta de NFP por parte cercana (la que permite el mayor paso
    en la direccion de descenso), dentro de una region de confianza; retorna la trayectoria"""
    P = pieces[ty[i]]
    X = W - P.w
    x = t[i].copy()
    c = np.array([slide if x[0] <= X/2 else -slide, 1.0])
    g = -c/np.linalg.norm(c)
    delta = max(P.w, P.h)
    path = [x.copy()]
    for _ in range(rounds):
        lo = np.array([max(0, x[0]-delta), max(0, x[1]-delta)]); hi = np.array([min(X, x[0]+delta), x[1]])
        A, B = [], []
        for j in placed:
            for a in range(len(P.parts)):
                for b in range(len(pieces[ty[j]].parts)):
                    V, n, d = nfp_c(ty[i], a, ty[j], b, pieces)
                    rel = x - t[j]
                    s = n @ rel - d
                    best, bs, bg = -1, -1, 0
                    for f in range(len(d)):
                        if s[f] < -1e-9: continue
                        ng = n[f] @ g
                        step = np.inf if ng >= -1e-12 else max(0, s[f])/(-ng)
                        if step > bs or (step == bs and ng > bg): best, bs, bg = f, step, ng
                    if best < 0: best = int(np.argmax(s))
                    # n.(x - tj) >= d  <=>  -n.x <= -d - n.tj
                    A.append(-n[best]); B.append(-d[best] - n[best] @ t[j])
        res = linprog(c, A_ub=np.array(A) if A else None, b_ub=np.array(B) if B else None,
                      bounds=list(zip(lo, hi)), method='highs')
        if res.status != 0: break
        x2 = res.x
        if c @ x2 >= c @ x - 1e-9: break
        x = x2
        path.append(x.copy())
    return np.array(path)


def construct(ty, pieces, W, grid=4, alpha=1, beta=5.53, gamma=0.77, record=()):
    """greedy: en cada paso, cada pieza restante desciende desde grid puntos sobre la carga; se elige por el
    criterio de BSG_PLP; record: pasos de los que se guardan las trayectorias"""
    n = len(ty)
    t = np.zeros((n, 2))
    placed, left = [], list(range(n))
    H0 = max(max(pieces[k].h for k in ty), sum(pieces[k].area for k in ty)/W)
    Vmax = max(pieces[k].area for k in ty)
    steps = []
    for step in range(n):
        Hcur = max([t[j][1] + pieces[ty[j]].h for j in placed], default=0)
        best, cand_paths = None, []
        for i in left:
            if any(ty[i] == ty[k] for k in left if k < i): continue      # piezas iguales: una sola vez
            P = pieces[ty[i]]
            for gx in np.linspace(0, W - P.w, grid):
                t[i] = (gx, Hcur + 0.05)
                path = descend(i, ty, t, placed, pieces, W)
                z = path[-1][1]
                sc = alpha*np.log(P.area/Vmax) - beta*z/H0 - gamma*max(0, z + P.h - Hcur)/H0
                cand_paths.append((i, path, sc))
                if best is None or sc > best[0]: best = (sc, i, path[-1])
        _, i, pos = best
        t[i] = pos
        if step in record: steps.append((step, [p for p in cand_paths if p[0] == i], i, t.copy(), list(placed)))
        placed.append(i); left.remove(i)
    return t, steps


def compact(ty, t0, pieces, W, max_iter=30, eps=1e-3):
    """compactacion global por LP: todas las traslaciones y H; una faceta de NFP por par de partes cercanas"""
    n = len(ty)
    t = t0.copy()
    delta = 0.5*min(min(pieces[k].w, pieces[k].h) for k in ty)
    H = max(t[i][1] + pieces[ty[i]].h for i in range(n))
    hist = [t.copy()]
    rows_last = None
    for _ in range(max_iter):
        A, B, rows = [], [], []
        nv = 2*n + 1
        for i in range(n):
            for j in range(i+1, n):
                rel = t[i] - t[j]
                g = np.array([0.0, -1.0 if t[i][1] >= t[j][1] else 1.0])
                for a in range(len(pieces[ty[i]].parts)):
                    for b in range(len(pieces[ty[j]].parts)):
                        V, nn, d = nfp_c(ty[i], a, ty[j], b, pieces)
                        if np.any(V.min(0) - 2*delta > rel) or np.any(V.max(0) + 2*delta < rel): continue
                        s = nn @ rel - d
                        best, bs, bg = -1, -1, 0
                        for f in range(len(d)):
                            if s[f] < -1e-9: continue
                            ng = nn[f] @ g
                            step = np.inf if ng >= -1e-12 else max(0, s[f])/(-ng)
                            if step > bs or (step == bs and ng > bg): best, bs, bg = f, step, ng
                        if best < 0: best = int(np.argmax(s))
                        row = np.zeros(nv)
                        row[2*i:2*i+2] = -nn[best]; row[2*j:2*j+2] = nn[best]
                        A.append(row); B.append(-d[best]); rows.append((i, j, a, b, best))
        for i in range(n):
            row = np.zeros(nv); row[2*i+1] = 1; row[-1] = -1
            A.append(row); B.append(-pieces[ty[i]].h)
        cost = np.zeros(nv); cost[-1] = 1; cost[1:2*n:2] = eps
        bounds = []
        for i in range(n):
            P = pieces[ty[i]]
            bounds.append((max(0, t[i][0]-delta), min(W-P.w, t[i][0]+delta)))
            bounds.append((max(0, t[i][1]-delta), t[i][1]+delta))
        bounds.append((0, None))
        res = linprog(cost, A_ub=np.array(A), b_ub=np.array(B), bounds=bounds, method='highs')
        if res.status != 0: break
        t2 = res.x[:2*n].reshape(n, 2)
        H2 = res.x[-1]
        if H2 >= H - 1e-7: break
        t, H, rows_last = t2, H2, rows
        hist.append(t.copy())
    return t, hist, rows_last


# ---------------- minimizacion del solapamiento ----------------
def axis_terms(i, k, ty, t, w, pieces, U):
    """terminos de phi(s) para mover la pieza i por el eje k: (j, peso, L, U, (c, nk)) por par de partes"""
    terms = []
    for j in range(len(ty)):
        if j == i: continue
        for a in range(len(pieces[ty[i]].parts)):
            for b in range(len(pieces[ty[j]].parts)):
                V, n, d = nfp_c(ty[i], a, ty[j], b, pieces)
                base = t[i] - t[j]; base[k] = -t[j][k]       # r(s) = base + s e_k
                c = d - n @ base; nk = n[:, k]
                L, R = -np.inf, np.inf
                ok = True
                for f in range(len(d)):
                    if abs(nk[f]) < 1e-12:
                        if c[f] <= 0: ok = False
                    elif nk[f] < 0: L = max(L, c[f]/nk[f])
                    else: R = min(R, c[f]/nk[f])
                if ok and L < R and R > 0 and L < U:
                    terms.append((j, w.get((min(i, j), max(i, j)), 1.0), L, R, c, nk))
    return terms


def phi(s, terms):
    return sum(wt*max(0.0, np.min(c - nk*s)) for (_, wt, _, _, c, nk) in terms)


def axis_move(i, k, ty, t, w, pieces, U):
    terms = axis_terms(i, k, ty, t, w, pieces, U)
    cand = {0.0, U, t[i][k]}
    for (_, _, L, R, _, _) in terms:
        for v in (L, R):
            if 0 <= v <= U: cand.add(v)
    cand = sorted(cand)
    vals = [phi(s, terms) for s in cand]
    m = min(vals)
    s = cand[vals.index(m)]
    if m >= phi(t[i][k], terms) - 1e-12: s = t[i][k]
    return s, terms, cand, vals


def total_pen(ty, t, pieces):
    n = len(ty)
    return sum(pair_pen(i, j, ty, t, pieces) for i in range(n) for j in range(i+1, n))


def minimize_overlap(ty, t, h, W, pieces, rng, passes=200, snap=(), record=None):
    """como Placer::minimize_overlap: pasadas sobre las piezas solapadas (orden aleatorio, ejes en orden aleatorio, movimiento
    exacto); si una pasada no reduce el solapamiento ponderado, se actualizan los pesos GLS. record: lista donde se
    guarda cada pasada (posiciones antes y despues, piezas movidas, solapamiento, pesos)"""
    n = len(ty)
    t = t.copy()
    w = {}
    snaps = [(0, t.copy(), total_pen(ty, t, pieces))]
    for it in range(1, passes+1):
        pen = np.zeros((n, n))
        for i in range(n):
            for j in range(i+1, n): pen[i, j] = pen[j, i] = pair_pen(i, j, ty, t, pieces)
        over = [i for i in range(n) if pen[i].sum() > TOL]
        if not over: it -= 1; break       # it: pasadas completadas
        before = t.copy()
        rng.shuffle(over)
        gain = 0.0
        for i in over:
            for k in rng.permutation(2):
                U = (W - pieces[ty[i]].w) if k == 0 else (h - pieces[ty[i]].h)
                old = t[i][k]
                s_, terms, _, _ = axis_move(i, k, ty, t, w, pieces, U)
                gain += phi(old, terms) - phi(s_, terms)
                t[i][k] = s_
        tot = total_pen(ty, t, pieces)
        wtot = sum(w.get((i, j), 1.0)*pair_pen(i, j, ty, t, pieces) for i in range(n) for j in range(i+1, n))
        if it in snap: snaps.append((it, t.copy(), tot))
        wupd = gain <= 1e-9
        if wupd:
            ov = {(i, j): pair_pen(i, j, ty, t, pieces) for i in range(n) for j in range(i+1, n)}
            mx = max(ov.values())
            for key, p in ov.items():
                w[key] = w.get(key, 1.0)*(1.2 + 0.8*p/mx) if p > TOL else max(1.0, w.get(key, 1.0)*0.95)
        if record is not None:
            wnew = sum(w.get((i, j), 1.0)*pair_pen(i, j, ty, t, pieces) for i in range(n) for j in range(i+1, n))
            record.append(dict(it=it, before=before, after=t.copy(), tot=tot, wtot=wtot, wnew=wnew, wupd=wupd, w=dict(w),
                               moved=[i for i in range(n) if np.linalg.norm(t[i] - before[i]) > 1e-9]))
    snaps.append((it, t.copy(), total_pen(ty, t, pieces)))
    return t, snaps


# ---------------- dibujo ----------------
def draw_layout(ax, ty, t, pieces, W, H=None, idx=None, alpha=1.0, edge=INK, lw=0.6, fill=True, labels=False,
                overlap=False):
    for i, polys in placed_polys(ty, t, pieces, idx):
        for P in polys:
            ax.add_patch(MplPolygon(P, closed=True, facecolor=COLORS[ty[i] % len(COLORS)] if fill else 'none',
                                    edgecolor=edge, lw=lw, alpha=alpha, joinstyle='round'))
        if labels:
            ctr = np.vstack(polys).mean(0)
            ax.text(*ctr, str(i+1), ha='center', va='center', fontsize=6, color=INK)
    if overlap:
        pl = placed_polys(ty, t, pieces)
        for x in range(len(pl)):
            for y in range(x+1, len(pl)):
                for A in pl[x][1]:
                    for B in pl[y][1]:
                        I = clip(A, B)
                        if I is not None and abs(0.5*cross2(I[1:-1]-I[0], I[2:]-I[0]).sum()) > 1e-6:
                            ax.add_patch(MplPolygon(I, closed=True, facecolor=RED, edgecolor='none', alpha=0.85))


def strip(ax, W, Hmax, H=None, hlabel=None):
    ax.plot([0, 0, W, W], [Hmax, 0, 0, Hmax], color=INK, lw=0.8)
    if H is not None:
        ax.axhline(H, color=ACCENT2, lw=0.8, ls='--')
        if hlabel: ax.text(W, H, ' ' + hlabel, color=ACCENT2, fontsize=7, va='center', ha='left')
    ax.set_xlim(-0.3, W + 1.2); ax.set_ylim(-0.3, Hmax)
    ax.set_aspect('equal'); ax.axis('off')


def height(ty, t, pieces):
    return max(t[i][1] + pieces[ty[i]].h for i in range(len(ty)))


# ---------------- ejemplo ----------------
def example():
    shapes = [
        [[(0, 0), (2.2, 0), (1.1, 1.6)]],                                       # triangulo
        [[(0, 0), (1.6, 0), (1.6, 1.0), (0, 1.0)]],                             # rectangulo
        [[(0, 0.5), (0.8, 0), (1.6, 0.5), (1.3, 1.4), (0.3, 1.4)]],             # pentagono
        [[(0, 0), (0.7, 0), (0.7, 1.8), (0, 1.8)], [(0, 0), (1.6, 0), (1.6, 0.6), (0, 0.6)]],   # L (dos partes)
        [[(0, 0), (1.0, 0.3), (0.6, 1.3)]],                                     # triangulo pequeno
        [[(0, 0.4), (0.4, 0), (1.2, 0), (1.6, 0.4), (1.2, 0.8), (0.4, 0.8)]],   # hexagono
    ]
    pieces = [Piece(s) for s in shapes]
    ty = [0, 1, 2, 3, 4, 5, 0, 2, 4, 1]
    return pieces, ty, 4.6


def fig_construction():
    pieces, ty, W = example()
    NFP.clear()
    t, steps = construct(ty, pieces, W, record=(1, 3, 6, 9))
    Hg = height(ty, t, pieces)
    Hmax = Hg + 1.0
    fig, axs = plt.subplots(1, 4, figsize=(7.2, 2.5))
    for k, (ax, (step, paths, i, tt, placed)) in enumerate(zip(axs, steps)):
        Hcur = max([tt[j][1] + pieces[ty[j]].h for j in placed], default=0)
        strip(ax, W, Hmax, Hcur, 'load' if k == 0 else None)
        draw_layout(ax, ty, tt, pieces, W, idx=placed)
        P = pieces[ty[i]]
        ref = np.array([P.w/2, P.h/2])       # trayectoria del centro de la caja de la pieza
        ib = int(np.argmax([p[2] for p in paths]))
        for m, (_, path, sc) in enumerate(paths):
            if m == ib: continue
            ax.plot(path[:, 0] + ref[0], path[:, 1] + ref[1], color=MUTED, lw=0.6, ls=':', zorder=5)
            ax.plot(*(path[0] + ref), 'o', ms=2.2, color=MUTED, zorder=6)
            for Q in P.parts:
                ax.add_patch(MplPolygon(Q + path[-1], closed=True, facecolor='none', edgecolor=MUTED, lw=0.5,
                                        ls=(0, (2, 1.5))))
        for Q in P.parts:
            ax.add_patch(MplPolygon(Q + tt[i], closed=True, facecolor=COLORS[ty[i] % len(COLORS)], edgecolor=ACCENT,
                                    lw=1.0, zorder=4))
        for m, (_, path, sc) in enumerate(paths):
            if m != ib: continue
            ax.plot(path[:, 0] + ref[0], path[:, 1] + ref[1], color=ACCENT, lw=1.1, zorder=7)
            ax.plot(*(path[0] + ref), 'o', ms=2.6, color=ACCENT, zorder=8)
        ax.set_title('(%s) piece %d of %d' % ('abcd'[k], step+1, len(ty)), fontsize=8, color=INK)
    fig.tight_layout(pad=0.3, w_pad=0.4)
    fig.savefig(os.path.join(OUT, 'construction.pdf'))
    fig.savefig(os.path.join(OUT, 'construction.png'), dpi=200)
    return pieces, ty, W, t, Hg


def fig_nfp(pieces):
    """N_AB: (a) lugar del punto de referencia de A al deslizar A alrededor de B; (b) N_AB = B + (-A);
    (c) penetracion"""
    fig, axs = plt.subplots(1, 3, figsize=(7.2, 2.7))
    A, B = pieces[4].parts[0], pieces[2].parts[0]
    tB = np.zeros(2)
    V, n, d = nfp(A, B)
    allc = np.vstack([A + v for v in V] + [B])
    lo = allc.min(0) - 0.1; hi = allc.max(0) + 0.1

    # (a) A desliza alrededor de B tocandolo; su punto de referencia (vertice inferior izquierdo) recorre N_AB
    ax = axs[0]
    ax.add_patch(MplPolygon(V, closed=True, facecolor='#eef3fb', edgecolor='none'))
    ax.add_patch(MplPolygon(B, closed=True, facecolor=COLORS[2], edgecolor=INK, lw=0.6, zorder=2))
    for k, v in enumerate(V):
        ax.add_patch(MplPolygon(A + v, closed=True, facecolor=COLORS[4], edgecolor=MUTED, lw=0.4, alpha=0.35, zorder=3))
    for k in range(len(V)):
        p, q = V[k], V[(k+1) % len(V)]
        ax.add_patch(FancyArrowPatch(p, q, arrowstyle='-|>', mutation_scale=6, color=ACCENT, lw=1.0, zorder=5,
                                     shrinkA=2, shrinkB=2))
    ax.plot(V[:, 0], V[:, 1], 'o', ms=3, color=ACCENT, zorder=6)
    k0 = int(np.argmin(V[:, 1]))
    ax.add_patch(MplPolygon(A + V[k0], closed=True, facecolor=COLORS[4], edgecolor=INK, lw=0.8, zorder=4))
    ax.text(*(V[k0] + A.mean(0) + np.array([0.0, -0.05])), '$A$', fontsize=8, ha='center', va='center', zorder=7)
    ax.annotate('reference\npoint of $A$', V[k0], V[k0] + np.array([0.75, -0.25]), fontsize=7, color=INK,
                va='center', arrowprops=dict(arrowstyle='-', lw=0.5, color=INK))
    ax.text(*(B.mean(0) + np.array([0.0, 0.05])), '$B$', fontsize=9, ha='center', va='center', zorder=7)
    vl = V[np.argmin(V[:, 0])]
    ax.text(*(vl + np.array([0.12, 0.35])), r'$N_{AB}$', color=ACCENT, fontsize=9)
    ax.set_xlim(lo[0], hi[0] + 0.3); ax.set_ylim(lo[1] - 0.35, hi[1]); ax.set_aspect('equal'); ax.axis('off')
    ax.set_title('(a) $A$ slides around $B$', fontsize=8, color=INK)

    # (b) N_AB = B + (-A): -A en cada vertice de B; la envolvente es N_AB
    ax = axs[1]
    ax.add_patch(MplPolygon(V, closed=True, facecolor='#eef3fb', edgecolor=ACCENT, lw=0.9, ls='--'))
    ax.add_patch(MplPolygon(B, closed=True, facecolor=COLORS[2], edgecolor=INK, lw=0.6, zorder=2))
    for b in B:
        ax.add_patch(MplPolygon(b - A, closed=True, facecolor=ACCENT2, alpha=0.12, edgecolor='none', zorder=3))
        ax.add_patch(MplPolygon(b - A, closed=True, facecolor='none', edgecolor=ACCENT2, lw=0.7, zorder=3))
        ax.plot(*b, 'o', ms=2.2, color=INK, zorder=4)
    ax.text(*(B.mean(0) + np.array([0.0, 0.05])), '$B$', fontsize=9, ha='center', va='center')
    bb = B[np.argmax(B[:, 1] - B[:, 0])]
    ax.text(*(bb - A.mean(0) + np.array([-0.12, 0.1])), '$-A$', fontsize=8, color=ACCENT2, ha='center',
            va='center', bbox=dict(facecolor='white', edgecolor='none', pad=0.5, alpha=0.8), zorder=8)
    ax.text(*(vl + np.array([0.12, 0.35])), r'$N_{AB}$', color=ACCENT, fontsize=9)
    ax.set_xlim(lo[0], hi[0] + 0.3); ax.set_ylim(lo[1] - 0.35, hi[1]); ax.set_aspect('equal'); ax.axis('off')
    ax.set_title(r'(b) $N_{AB}=B\oplus(-A)$', fontsize=8, color=INK)

    # (a) NFP y penetracion: el triangulo A (movil) solapa el pentagono B (fijo)
    ax = axs[2]
    A, B = pieces[4].parts[0], pieces[2].parts[0]
    tB = np.array([0.0, 0.0]); tA = np.array([0.5, 0.9])
    V, n, d = nfp(A, B)
    r = tA - tB
    sl = d - n @ r
    f = int(np.argmin(sl)); delta = sl[f]
    ax.add_patch(MplPolygon(V + tB, closed=True, facecolor='#eef3fb', edgecolor=ACCENT, lw=0.9, ls='--'))
    ax.add_patch(MplPolygon(B + tB, closed=True, facecolor=COLORS[2], edgecolor=INK, lw=0.6))
    ax.add_patch(MplPolygon(A + tA, closed=True, facecolor=COLORS[4], edgecolor=INK, lw=0.6, alpha=0.9))
    ax.add_patch(MplPolygon(clip(A + tA, B + tB), closed=True, facecolor=RED, edgecolor='none'))
    sep = tA + delta*n[f]
    ax.add_patch(MplPolygon(A + sep, closed=True, facecolor='none', edgecolor=INK, lw=0.6, ls=(0, (2, 1.5))))
    ax.plot(*tA, 'o', ms=3, color=INK, zorder=6)
    ax.plot(*sep, 'o', ms=2.5, color=INK, mfc='white', zorder=6)
    ax.add_patch(FancyArrowPatch(tA, sep, arrowstyle='-|>', mutation_scale=7, color=RED, lw=1.1, zorder=7,
                                 shrinkA=0, shrinkB=1))
    mid = tA + 0.5*delta*n[f]
    ax.text(*(mid + np.array([-0.1, 0.05])), r'$\delta_{AB}(r)$', color=RED, fontsize=8, ha='right', va='center',
            bbox=dict(facecolor='white', edgecolor='none', pad=0.5, alpha=0.8), zorder=8)
    ax.text(*(tA + np.array([0.06, -0.1])), r'$r$', fontsize=8, ha='left', va='top')
    ax.text(*(B.mean(0) + tB + np.array([0.15, -0.35])), '$B$', fontsize=9)
    ax.text(*(A.max(0) + tA + np.array([-0.15, 0.08])), '$A$', fontsize=9)
    ax.text(*(sep + np.array([0.0, 0.85])), '$A$ separated', fontsize=7, color=MUTED, ha='right')
    vl = (V + tB)[np.argmin((V + tB)[:, 0])]
    ax.text(*(vl + np.array([0.08, 0.3])), r'$N_{AB}$', color=ACCENT, fontsize=9)
    allp = np.vstack([V + tB, A + tA, A + sep])
    lo = allp.min(0) - 0.15; hi = allp.max(0) + 0.15
    ax.set_xlim(lo[0], hi[0]); ax.set_ylim(lo[1], hi[1]); ax.set_aspect('equal'); ax.axis('off')
    ax.set_title('(c) penetration', fontsize=8, color=INK)

    fig.tight_layout(pad=0.3, w_pad=0.6)
    fig.savefig(os.path.join(OUT, 'nfp.pdf'))
    fig.savefig(os.path.join(OUT, 'nfp.png'), dpi=200)


def fig_axis(pieces):
    """Proposicion 1: (a) recta r(s) que cruza N_AB; (b) d_f - n_f.r(s) por faceta (afines), su minimo y
    delta_AB(s) = max(0, min): cero fuera de (L, U) y concava dentro; (c) suma ponderada de dos terminos: concava
    entre puntos de quiebre consecutivos, minimo en un punto de quiebre"""
    A, B = pieces[4].parts[0], pieces[2].parts[0]
    V, n, d = nfp(A, B)
    y0 = 0.55
    fcol = plt.get_cmap('tab10')
    nf = len(d)
    fig = plt.figure(figsize=(7.2, 4.9))
    top, bot = fig.subfigures(2, 1, height_ratios=[1, 1.2], hspace=0.02)
    ax_p, ax_n, ax_d = top.subplots(1, 3, gridspec_kw={'width_ratios': [1.0, 0.9, 1.0]})
    ax_g, ax_phi = bot.subplots(1, 2, gridspec_kw={'width_ratios': [1.45, 1.0]})
    axs = [ax_n, ax_d, ax_g, ax_phi]

    # intervalo (L, U) sobre la recta r(s) = (s, y0)
    c = d - n[:, 1]*y0; nk = n[:, 0]
    L = max(c[f]/nk[f] for f in range(nf) if nk[f] < -1e-12)
    U = min(c[f]/nk[f] for f in range(nf) if nk[f] > 1e-12)
    S = np.linspace(V[:, 0].min() - 0.5, V[:, 0].max() + 0.5, 600)

    # (a)
    ax = axs[0]
    ax.add_patch(MplPolygon(V, closed=True, facecolor='#eef3fb', edgecolor='none'))
    for f in range(nf):
        # arista de la faceta f: vertices con n_f.v = d_f
        on = V[np.abs(V @ n[f] - d[f]) < 1e-9]
        if len(on) >= 2: ax.plot(on[:, 0], on[:, 1], color=fcol(f), lw=1.6, solid_capstyle='round')
    ax.plot([S[0], S[-1]], [y0, y0], color=INK, lw=0.8)
    ax.annotate('', (S[-1], y0), (S[-1] - 0.01, y0), arrowprops=dict(arrowstyle='-|>', color=INK, lw=0.8))
    ax.text(S[-1], y0 + 0.08, '$r(s)$', fontsize=8, ha='right', va='bottom')
    for v, lab in ((L, '$L$'), (U, '$U$')):
        ax.plot(v, y0, 'o', ms=3.5, color=INK, mfc='white', zorder=5)
        ax.text(v, y0 - 0.12, lab, fontsize=8, ha='center', va='top')
    sm = 0.35*L + 0.65*U
    rm = np.array([sm, y0])
    gap = d - n @ rm; f0 = int(np.argmin(gap))
    ax.plot(*rm, 'o', ms=3, color=RED, zorder=6)
    ax.add_patch(FancyArrowPatch(rm, rm + gap[f0]*n[f0], arrowstyle='-|>', mutation_scale=6, color=RED, lw=1.0,
                                 zorder=6, shrinkA=0, shrinkB=0))
    ax.text(*(rm + 0.5*gap[f0]*n[f0] + np.array([0.08, 0.0])), r'$\delta$', color=RED, fontsize=8, va='center')
    ax.text(*(V.mean(0) + np.array([-0.1, -0.45])), '$N_{AB}$', color=ACCENT, fontsize=9, ha='center')
    ax.set_xlim(S[0], S[-1] + 0.05); ax.set_aspect('equal'); ax.axis('off')

    # piezas: B fija y A con su punto de referencia en r(L), r(s) y r(U) (A toca a B en L y en U)
    ax = ax_p
    ax.add_patch(MplPolygon(B, closed=True, facecolor=COLORS[2], edgecolor=INK, lw=0.6, zorder=2))
    ax.text(*(B.mean(0) + np.array([0.0, -0.45])), '$B$', fontsize=9, ha='center', va='center', zorder=7)
    for v, lab in ((L, '$s=L$'), (U, '$s=U$')):
        ax.add_patch(MplPolygon(A + np.array([v, y0]), closed=True, facecolor='none', edgecolor=INK, lw=0.6,
                                ls=(0, (2, 1.2)), zorder=3))
        ax.text(*(A.max(0) + np.array([v, y0]) + np.array([-0.15, 0.06])), lab, fontsize=7, ha='center', zorder=7)
    ax.add_patch(MplPolygon(A + rm, closed=True, facecolor=COLORS[4], edgecolor=INK, lw=0.6, zorder=3))
    I = clip(A + rm, B)
    if I is not None: ax.add_patch(MplPolygon(I, closed=True, facecolor=RED, edgecolor='none', zorder=4))
    ax.text(*(A.max(0) + rm + np.array([0.05, 0.06])), '$A$', fontsize=9, zorder=7)
    ax.plot([S[0], S[-1]], [y0, y0], color=INK, lw=0.6, ls=':')
    for v in (L, U): ax.plot(v, y0, 'o', ms=3, color=INK, mfc='white', zorder=6)
    ax.plot(*rm, 'o', ms=3, color=RED, zorder=6)
    allp = np.vstack([B, A + np.array([L, y0]), A + np.array([U, y0])])
    ax.set_xlim(S[0], S[-1]); ax.set_ylim(allp[:, 1].min() - 0.1, allp[:, 1].max() + 0.25)
    ax.set_aspect('equal'); ax.axis('off')
    ax.set_title('(a) $A$ moves along $x$, $B$ fixed', fontsize=8, color=INK)
    ax_n.set_title('(b) $r(s)$ crosses $N_{AB}$', fontsize=8, color=INK)

    # (b)
    ax = axs[1]
    G = np.array([c[f] - nk[f]*S for f in range(nf)])
    for f in range(nf):
        ax.plot(S, G[f], color=fcol(f), lw=0.8, alpha=0.9)
    m = G.min(0)
    ax.plot(S, np.maximum(0, m), color=INK, lw=1.8)
    ax.fill_between(S, 0, np.maximum(0, m), color=GRID, alpha=0.8)
    ax.axhline(0, color=MUTED, lw=0.6)
    for v, lab in ((L, '$L$'), (U, '$U$')):
        ax.axvline(v, color=MUTED, lw=0.6, ls=':')
        ax.text(v, -0.52, lab, fontsize=8, ha='center', va='top')
    ax.set_ylim(-0.5, 1.25); ax.set_xlim(S[0], S[-1])
    ax.set_xticks([]); ax.set_xlabel('$s$'); ax.set_yticks([0])
    ax.text(S[-1] - 0.05, -0.45, r'thin: $d_f-n_f\cdot r(s)$, one per facet', fontsize=6.5, color=MUTED,
            ha='right', va='bottom')
    ax.text(0.5*(L + U), 0.9, r'$\delta_{AB}(r(s))$', fontsize=8, ha='center', va='bottom',
            bbox=dict(facecolor='white', edgecolor='none', pad=0.5, alpha=0.85))
    ax.set_title('(c) $\\delta_{AB}$ along the line', fontsize=8, color=INK)

    # (c) dos terminos (dos piezas fijas) con pesos, en el rango del contenedor [0, U_k]: phi concava entre puntos
    # de quiebre consecutivos y minimo en uno de ellos
    ax = axs[3]
    off2 = np.array([1.9, 0.25]); w1, w2 = 1.0, 1.6
    def term(off, Ss):
        dd = d + n @ off; cc = dd - n[:, 1]*y0
        dl = np.maximum(0, np.min(cc[:, None] - nk[:, None]*Ss[None, :], axis=0))
        Lx = max(cc[f]/nk[f] for f in range(nf) if nk[f] < -1e-12)
        Ux = min(cc[f]/nk[f] for f in range(nf) if nk[f] > 1e-12)
        return dl, Lx, Ux
    _, L1, U1 = term(np.zeros(2), S[:1]); _, L2, U2 = term(off2, S[:1])
    s0, s1 = L1 + 0.5*(U1 - L1), L2 + 0.6*(U2 - L2)        # limites del contenedor para la pieza movil
    pad = 0.12*(s1 - s0)
    S2 = np.linspace(s0 - pad, s1 + pad, 800)
    d1 = term(np.zeros(2), S2)[0]; d2 = term(off2, S2)[0]
    inside = (S2 >= s0) & (S2 <= s1)
    ax.axvspan(S2[0], s0, color=GRID, alpha=0.6, lw=0); ax.axvspan(s1, S2[-1], color=GRID, alpha=0.6, lw=0)
    ax.plot(S2, w1*d1, color=ACCENT, lw=0.9, label=r'$w_1\delta_1$')
    ax.plot(S2, w2*d2, color=ACCENT2, lw=0.9, label=r'$w_2\delta_2$')
    ax.plot(S2[inside], (w1*d1 + w2*d2)[inside], color=INK, lw=1.8, label=r'$\phi$')
    bps = sorted([s0, s1] + [b for b in (L1, U1, L2, U2) if s0 < b < s1])
    vals = [w1*term(np.zeros(2), np.array([b]))[0][0] + w2*term(off2, np.array([b]))[0][0] for b in bps]
    for b in bps: ax.axvline(b, color=MUTED, lw=0.6, ls=':', zorder=0)
    ax.plot(bps, vals, 'o', ms=3, color=INK, zorder=5)
    k = int(np.argmin(vals))
    ax.plot([bps[k]], [vals[k]], '*', ms=9, color=ACCENT, zorder=6)
    ax.set_xticks([s0, s1]); ax.set_xticklabels(['$0$', '$U_k$'])
    ax.set_xlabel('$s$'); ax.set_ylim(0, max(vals)*1.45); ax.set_xlim(S2[0], S2[-1])
    ax.legend(fontsize=7, frameon=False, loc='upper center', ncol=3, handlelength=1.2, columnspacing=0.8)
    ax.set_title(r'(e) $\phi$: minimum at a breakpoint', fontsize=8, color=INK)
    smin = bps[k]

    # (c) las piezas: dos fijas (j = 1, 2) y la pieza movil A en la posicion elegida; limites del contenedor
    ax = axs[2]
    for off, lab in ((np.zeros(2), '$j=1$, $w_1=%g$' % w1), (off2, '$j=2$, $w_2=%g$' % w2)):
        ax.add_patch(MplPolygon(B + off, closed=True, facecolor=COLORS[2], edgecolor=INK, lw=0.5, zorder=2))
        ax.text(B.mean(0)[0] + off[0], B[:, 1].min() + off[1] - 0.04, lab, fontsize=7, ha='center', va='top',
                zorder=7)
    # puntos de quiebre (los mismos de (e)) sobre la recta del punto de referencia de A
    ax.plot([s0, s1], [y0, y0], color=INK, lw=0.6, ls=':', zorder=6)
    ax.plot(bps, [y0]*len(bps), 'o', ms=3, color=INK, zorder=8)
    for bpt, val in zip(bps, vals):        # phi = w1 delta1 + w2 delta2 en cada punto (los mismos valores de (e))
        ax.text(bpt, y0 - 0.07, r'$\phi=%.2f$' % val, fontsize=6.5, ha='center', va='top', zorder=11,
                color=ACCENT if abs(bpt - smin) < 1e-9 else INK,
                bbox=dict(facecolor='white', edgecolor='none', pad=0.2, alpha=0.85))
    ax.plot([smin], [y0], '*', ms=9, color=ACCENT, zorder=9)

    def pen_vec(off, r):
        """vector de separacion minima de A en r respecto de la pieza fija en off (cero si no se solapan)"""
        dd = d + n @ off
        g = dd - n @ r
        f = int(np.argmin(g))
        return max(0.0, g[f])*n[f]

    # A en cada punto de quiebre (sin relleno) y su penetracion respecto de cada pieza que solapa
    for bpt in bps:
        r = np.array([bpt, y0])
        best = abs(bpt - smin) < 1e-9
        ax.add_patch(MplPolygon(A + r, closed=True, facecolor='none', edgecolor=ACCENT if best else INK,
                                lw=1.1 if best else 0.6, ls='-' if best else (0, (2.5, 1.5)), zorder=7))
        for off, col, lab in ((np.zeros(2), ACCENT, r'$\delta_1$'), (off2, ACCENT2, r'$\delta_2$')):
            v = pen_vec(off, r)
            if np.linalg.norm(v) < 1e-9: continue
            ax.add_patch(FancyArrowPatch(r, r + v, arrowstyle='-|>', mutation_scale=7, color=col, lw=1.2, zorder=10,
                                         shrinkA=0, shrinkB=0))
            ax.text(*(r + v + 0.07*v/np.linalg.norm(v)), lab, color=col, fontsize=8, ha='center', va='center',
                    zorder=11, bbox=dict(facecolor='white', edgecolor='none', pad=0.2, alpha=0.8))
    yb = min(B[:, 1].min(), (B + off2)[:, 1].min()) - 0.32
    ax.annotate('', (s1, yb), (s0, yb), arrowprops=dict(arrowstyle='<->', color=INK, lw=0.6))
    ax.text(0.5*(s0 + s1), yb - 0.05, 'range of the reference point of $A$ ($0\\ldots U_k$)', fontsize=6.5,
            ha='center', va='top')
    ax.text(*(A[2] + np.array([smin, y0]) + np.array([-0.06, -0.02])), '$A$', fontsize=8, ha='right', color=ACCENT,
            zorder=7)
    allp = np.vstack([B, B + off2, A + np.array([s0, y0]), A + np.array([s1, y0])])
    ax.set_xlim(allp[:, 0].min() - 0.05, allp[:, 0].max() + 0.05); ax.set_ylim(yb - 0.3, allp[:, 1].max() + 0.05)
    ax.set_aspect('equal'); ax.axis('off')
    ax.set_title('(d) $A$ between two fixed pieces', fontsize=8, color=INK)
    fig.savefig(os.path.join(OUT, 'axis.pdf'), bbox_inches='tight')
    fig.savefig(os.path.join(OUT, 'axis.png'), dpi=200, bbox_inches='tight')


def descend_trace(i, ty, t, placed, pieces, W, slide=0.065, rounds=50):
    """como descend, pero registra cada ronda: punto, region de confianza, restricciones (a.u <= b, pieza j) y
    solucion del LP"""
    P = pieces[ty[i]]
    X = W - P.w
    x = t[i].copy()
    c = np.array([slide if x[0] <= X/2 else -slide, 1.0])
    g = -c/np.linalg.norm(c)
    delta = max(P.w, P.h)
    out = []
    for _ in range(rounds):
        lo = np.array([max(0, x[0]-delta), max(0, x[1]-delta)]); hi = np.array([min(X, x[0]+delta), x[1]])
        cons = []
        for j in placed:
            for a in range(len(P.parts)):
                for b in range(len(pieces[ty[j]].parts)):
                    V, n, d = nfp_c(ty[i], a, ty[j], b, pieces)
                    # region de confianza (cajas): la parte puede tocar a b?
                    if np.any(V.max(0) + t[j] < lo - 1e-9) or np.any(V.min(0) + t[j] > hi + 1e-9): continue
                    rel = x - t[j]
                    sl = n @ rel - d
                    best, bs, bg = -1, -1, 0
                    for f in range(len(d)):
                        if sl[f] < -1e-9: continue
                        ng = n[f] @ g
                        step = np.inf if ng >= -1e-12 else max(0, sl[f])/(-ng)
                        if step > bs or (step == bs and ng > bg): best, bs, bg = f, step, ng
                    if best < 0: best = int(np.argmax(sl))
                    cons.append((-n[best], -d[best] - n[best] @ t[j], j))
        res = linprog(c, A_ub=np.array([q[0] for q in cons]) if cons else None,
                      b_ub=np.array([q[1] for q in cons]) if cons else None, bounds=list(zip(lo, hi)), method='highs')
        x2 = res.x if res.status == 0 else x
        improved = c @ x2 < c @ x - 1e-9
        out.append(dict(x=x.copy(), lo=lo, hi=hi, cons=cons, x2=x2.copy(), improved=improved, c=c))
        if not improved: break
        x = x2
    return out


def halfplane_clip(poly, a, b):
    """{u in poly : a.u <= b}"""
    out = []
    m = len(poly)
    for k in range(m):
        p, q = poly[k], poly[(k+1) % m]
        fp, fq = a @ p - b, a @ q - b
        if fp <= 0: out.append(p)
        if fp*fq < 0: out.append(p + (q - p)*fp/(fp - fq))
    return np.array(out) if len(out) >= 2 else None


def descend_scene(yl, yr, W=6.0, x0=4.2, z0=3.9):
    """plataforma con la cara superior inclinada (de yl a yr), una pendiente apoyada en ella y un cuadrado que cae"""
    yB = lambda x: yl + (yr - yl)*x/W
    B = [(0, 0), (W, 0), (W, yr), (0, yl)]
    A = [(2.6, yB(2.6)), (W, yr), (W, yr + 2.6)]
    sq = [(0, 0), (0.7, 0), (0.7, 0.7), (0, 0.7)]
    pieces = [Piece([B]), Piece([A]), Piece([sq])]
    t = np.array([[0, 0], [2.6, min(yB(2.6), yr)], [x0, z0]], float)
    NFP.clear()
    return pieces, t, descend_trace(2, [0, 1, 2], t, [0, 1], pieces, W)


def fig_descend():
    """un Descend paso a paso: el cuadrado cae, desliza por una pendiente y llega a una plataforma con inclinacion leve
    en el mismo sentido (sigue deslizando hasta la pared) o en sentido contrario (se detiene en la V)"""
    W = 6.0
    FCOL = {0: '#5b8c3a', 1: ACCENT2}           # color de las facetas de la plataforma y de la pendiente
    SCOL = {0: '#d9d6cc', 1: COLORS[0]}
    fig, axs = plt.subplots(2, 3, figsize=(7.2, 5.0))
    xl, yl_ = (-0.15, W + 0.15), (-0.15, 4.75)

    def base(ax, pieces, t):
        ax.plot([0, 0, W, W], [yl_[1], 0, 0, yl_[1]], color=INK, lw=0.8)
        for j in (0, 1):
            ax.add_patch(MplPolygon(pieces[j].parts[0] + t[j], closed=True, facecolor=SCOL[j], edgecolor=INK, lw=0.6))
        ax.set_xlim(*xl); ax.set_ylim(*yl_); ax.set_aspect('equal', adjustable='box'); ax.axis('off')

    def traj(ax, pieces, t, tr, title):
        base(ax, pieces, t)
        P = pieces[2]
        pts = [tr[0]['x']] + [R['x2'] for R in tr if R['improved']]
        for k, q in enumerate(pts):
            last = k == len(pts) - 1
            ax.add_patch(MplPolygon(P.parts[0] + q, closed=True, facecolor=COLORS[1] if last else 'none',
                                    edgecolor=INK if last or k == 0 else MUTED, lw=0.6,
                                    ls='-' if last or k == 0 else (0, (2, 1.5)), zorder=4))
        pts = np.array(pts)
        ax.plot(pts[:, 0], pts[:, 1], '-o', color=ACCENT, ms=2.5, lw=1.0, zorder=6)
        ax.set_title(title, fontsize=8, color=INK)

    def lp(ax, pieces, t, R, k, title):
        base(ax, pieces, t)
        P = pieces[2]
        box = np.array([R['lo'], [R['hi'][0], R['lo'][1]], R['hi'], [R['lo'][0], R['hi'][1]]])
        reg = box
        for (a_, b_, j) in R['cons']:
            if reg is not None: reg = halfplane_clip(reg, a_, b_)
        ax.add_patch(MplPolygon(box, closed=True, facecolor='none', edgecolor=MUTED, lw=0.7, ls=(0, (3, 2)), zorder=5))
        if reg is not None and len(reg) >= 3:
            ax.add_patch(MplPolygon(reg, closed=True, facecolor=ACCENT, alpha=0.3, edgecolor=ACCENT, lw=0.9, zorder=6))
        for (a_, b_, j) in R['cons']:
            nrm = a_/np.linalg.norm(a_); p0 = nrm*(b_/np.linalg.norm(a_)); dv = np.array([-nrm[1], nrm[0]])
            seg = np.array([p0 - 12*dv, p0 + 12*dv])
            active = abs(b_ - a_ @ R['x2']) < 1e-6
            ax.plot(seg[:, 0], seg[:, 1], color=FCOL[j], lw=1.5 if active else 0.9, ls='-' if active else (0, (4, 2)),
                    zorder=7)
        ax.add_patch(MplPolygon(P.parts[0] + R['x'], closed=True, facecolor='none', edgecolor=INK, lw=0.9, zorder=8))
        if R['improved']:
            ax.add_patch(MplPolygon(P.parts[0] + R['x2'], closed=True, facecolor='none', edgecolor=ACCENT, lw=1.1,
                                    zorder=8))
            ax.add_patch(FancyArrowPatch(R['x'], R['x2'], arrowstyle='-|>', mutation_scale=7, color=ACCENT, lw=1.0,
                                         zorder=10, shrinkA=2, shrinkB=2))
            ax.plot(*R['x2'], 'o', ms=3, color=ACCENT, zorder=11)
        ax.plot(*R['x'], 'o', ms=3, color=INK, zorder=11)
        ax.set_title('(%s) LP %d: %s' % (k, title[0], title[1]), fontsize=8, color=INK)
        # zoom alrededor de la region de confianza
        c0 = 0.5*(R['lo'] + R['hi']) + np.array([0.35, 0.3]); half = 1.45
        ax.set_xlim(c0[0] - half, c0[0] + half); ax.set_ylim(max(c0[1] - half, -0.15), c0[1] + half)
        ax.set_aspect('equal', adjustable='box')
        for ln in ax.lines: ln.set_clip_on(True)

    pieces, t, tr = descend_scene(0.4, 1.0)
    traj(axs[0, 0], pieces, t, tr, '(a) same inclination: %d LPs' % len(tr))
    land = next(k for k, R in enumerate(tr) if any(j == 0 and abs(b_ - a_ @ R['x2']) < 1e-6 for a_, b_, j in R['cons']))
    lp(axs[0, 1], pieces, t, tr[land - 1], 'b', (land, 'along the slope'))
    lp(axs[0, 2], pieces, t, tr[land + 1], 'c', (land + 2, 'along the platform'))
    pieces, t, tr = descend_scene(1.0, 0.4)
    traj(axs[1, 0], pieces, t, tr, '(d) opposite inclination: %d LPs' % len(tr))
    v = next(k for k, R in enumerate(tr) if sum(abs(b_ - a_ @ R['x2']) < 1e-6 for a_, b_, j in R['cons']) >= 2)
    lp(axs[1, 1], pieces, t, tr[v], 'e', (v + 1, 'reaches the V'))
    lp(axs[1, 2], pieces, t, tr[-1], 'f', (len(tr), 'stop'))
    c = tr[0]['c']; o = np.array([0.6, 4.4])
    axs[0, 0].add_patch(FancyArrowPatch(o, o - 0.6*c/np.linalg.norm(c), arrowstyle='-|>', mutation_scale=7, color=INK,
                                        lw=0.8, zorder=9))
    axs[0, 0].text(*(o + np.array([0.15, -0.3])), '$-c$', fontsize=8)
    fig.tight_layout(pad=0.3, w_pad=0.4, h_pad=0.6)
    fig.subplots_adjust(top=0.95, hspace=0.18)
    fig.savefig(os.path.join(OUT, 'descend.pdf'))
    fig.savefig(os.path.join(OUT, 'descend.png'), dpi=200)


def fig_overlap(pieces, ty, W, t, H):
    NFP.clear()
    fig = plt.figure(figsize=(7.2, 4.6))
    top, bot = fig.subfigures(2, 1, height_ratios=[1.05, 1], hspace=0.04)
    ax_b, ax_c = top.subplots(1, 2, gridspec_kw={'width_ratios': [1.0, 1.25]})

    # capa solapada: la solucion greedy con la altura objetivo reducida (como en la oscilacion)
    h = 0.9*H
    tt = t.copy()
    for i in range(len(ty)):
        tt[i][1] = min(tt[i][1], h - pieces[ty[i]].h)

    # (b) movimiento exacto por el eje x de la pieza con mayor solapamiento
    pens = [sum(pair_pen(i, j, ty, tt, pieces) for j in range(len(ty)) if j != i) for i in range(len(ty))]
    i = int(np.argmax(pens))
    U = W - pieces[ty[i]].w
    s_new, terms, cand, vals = axis_move(i, 0, ty, tt, {}, pieces, U)
    ax = ax_b
    strip(ax, W, h + 0.35, h, '$h$')
    draw_layout(ax, ty, tt, pieces, W, idx=[j for j in range(len(ty)) if j != i])
    t2 = tt[i].copy(); t2[0] = s_new
    for Q in pieces[ty[i]].parts:
        ax.add_patch(MplPolygon(Q + tt[i], closed=True, facecolor=COLORS[ty[i] % len(COLORS)], edgecolor=INK, lw=0.6,
                                zorder=4))
        ax.add_patch(MplPolygon(Q + t2, closed=True, facecolor='none', edgecolor=ACCENT, lw=1.1, zorder=5))
    for x in range(len(ty)):
        if x == i: continue
        for Pa in placed_polys(ty, tt, pieces, [i])[0][1]:
            for Pb in placed_polys(ty, tt, pieces, [x])[0][1]:
                I = clip(Pa, Pb)
                if I is not None: ax.add_patch(MplPolygon(I, closed=True, facecolor=RED, edgecolor='none', zorder=4))
    ymid = tt[i][1] + 0.35*pieces[ty[i]].h
    ax.add_patch(FancyArrowPatch((tt[i][0] + 0.5*pieces[ty[i]].w, ymid), (s_new + 0.5*pieces[ty[i]].w, ymid),
                                 arrowstyle='-|>', mutation_scale=7, color=ACCENT, lw=1.0, zorder=6))
    ax.text(tt[i][0] + 0.75*pieces[ty[i]].w, tt[i][1] + 0.6*pieces[ty[i]].h, '$i$', fontsize=9, zorder=7)
    ax.set_title('(a) move of piece $i$ along $x$', fontsize=8, color=INK)

    # (c) phi(s): suma de terminos concavos en intervalos; se evalua solo en los extremos
    ax = ax_c
    S = np.linspace(0, U, 800)
    by_j = {}
    for term in terms: by_j.setdefault(term[0], []).append(term)
    for j, tj in sorted(by_j.items()):
        ax.plot(S, [phi(s, tj) for s in S], lw=0.9, color=COLORS[ty[j] % len(COLORS)])
    ax.plot(S, [phi(s, terms) for s in S], color=INK, lw=1.3, label=r'$\phi(s)$')
    for c_ in cand: ax.axvline(c_, color=GRID, lw=0.6, zorder=0)
    ax.plot(cand, vals, 'o', ms=2.8, color=INK, zorder=5, label='breakpoints (evaluated)')
    ax.plot([tt[i][0]], [phi(tt[i][0], terms)], 's', ms=4, color=RED, zorder=6, label='current position')
    ax.plot([s_new], [phi(s_new, terms)], '*', ms=8, color=ACCENT, zorder=6, label='new position')
    ax.set_xlabel('$s$ (position of $i$ along $x$)'); ax.set_ylabel('weighted penetration')
    ax.set_ylim(0, max(vals)*1.55)
    ax.legend(fontsize=6, frameon=False, loc='upper left', handlelength=1.4, ncol=2, columnspacing=0.8)
    ax.set_title('(b) overlap of $i$ along $x$', fontsize=8, color=INK)

    # (d)-(g) minimizacion del solapamiento hasta separar, y compactacion por LP
    rng = np.random.default_rng(3)
    t_end, snaps = minimize_overlap(ty, tt, h, W, pieces, rng, snap=(1, 3))
    tc, _, _ = compact(ty, t_end, pieces, W)
    Hc = height(ty, tc, pieces)
    axs = bot.subplots(1, 4)
    show = [snaps[0], snaps[1], snaps[-1]]
    for k, (it, ts, tot) in enumerate(show):
        ax = axs[k]
        strip(ax, W, H + 0.3, h, '$h$')
        ax.axhline(H, color=MUTED, lw=0.6, ls=':')
        draw_layout(ax, ty, ts, pieces, W, overlap=True)
        lab = 'start' if it == 0 else ('after pass %d' % it)
        ax.set_title('(%s) %s\n$\\sum\\delta=%.2f$' % ('cde'[k], lab, tot), fontsize=8, color=INK)
        if k == 0: ax.text(W, H, ' $H$', color=MUTED, fontsize=7, va='center')
    ax = axs[3]
    strip(ax, W, H + 0.3, h, '$h$')
    ax.axhline(H, color=MUTED, lw=0.6, ls=':')
    draw_layout(ax, ty, t_end, pieces, W, fill=False, edge=MUTED, lw=0.5)
    draw_layout(ax, ty, tc, pieces, W)
    for i in range(len(ty)):
        P = pieces[ty[i]]; ref = np.array([P.w/2, P.h/2])
        if np.linalg.norm(tc[i] - t_end[i]) > 0.04:
            ax.add_patch(FancyArrowPatch(t_end[i] + ref, tc[i] + ref, arrowstyle='-|>', mutation_scale=6, color=ACCENT,
                                         lw=0.8, zorder=6, shrinkA=0, shrinkB=0))
    ax.axhline(Hc, color=ACCENT, lw=0.8, ls='--')
    ax.set_title('(f) LP compaction\nheight %.2f $\\to$ %.2f' % (height(ty, t_end, pieces), Hc), fontsize=8, color=INK)
    fig.savefig(os.path.join(OUT, 'overlap.pdf'), bbox_inches='tight')
    fig.savefig(os.path.join(OUT, 'overlap.png'), dpi=200, bbox_inches='tight')
    print('altura greedy %.3f, objetivo %.3f, pasadas %d, solapamiento final %.2g, altura compactada %.3f'
          % (H, h, snaps[-1][0], snaps[-1][2], Hc))


def fig_passes(pieces, ty, W, t, H):
    """todas las pasadas de MinimizeOverlap de la figura overlap: piezas movidas (antes punteado, flecha), solapamientos y
    pesos GLS de los pares (lineas entre centros, grosor segun el peso)"""
    NFP.clear()
    h = 0.9*H
    tt = t.copy()
    for i in range(len(ty)):
        tt[i][1] = min(tt[i][1], h - pieces[ty[i]].h)
    rng = np.random.default_rng(3)
    rec = []
    minimize_overlap(ty, tt, h, W, pieces, rng, record=rec)
    t0 = total_pen(ty, tt, pieces)
    panels = [dict(it=0, after=tt, tot=t0, wtot=t0, wnew=t0, moved=[], wupd=False, w={})] + rec
    ncol = 5
    nrow = int(np.ceil(len(panels)/ncol))
    fig, axs = plt.subplots(nrow, ncol, figsize=(7.2, 2.05*nrow))
    axs = np.atleast_2d(axs)
    for k, ax in enumerate(axs.flat):
        if k >= len(panels): ax.axis('off'); continue
        R = panels[k]
        strip(ax, W, h + 0.3, h, None)
        ax.set_xlim(-0.2, W + 0.2)
        draw_layout(ax, ty, R['after'], pieces, W, overlap=True)
        for i in R['moved']:
            P = pieces[ty[i]]; ref = np.array([P.w/2, P.h/2])
            for Q in P.parts:
                ax.add_patch(MplPolygon(Q + R['before'][i], closed=True, facecolor='none', edgecolor=ACCENT, lw=0.6,
                                        ls=(0, (2, 1.2)), zorder=6))
                ax.add_patch(MplPolygon(Q + R['after'][i], closed=True, facecolor='none', edgecolor=ACCENT, lw=1.0,
                                        zorder=6))
            ax.add_patch(FancyArrowPatch(R['before'][i] + ref, R['after'][i] + ref, arrowstyle='-|>', mutation_scale=5,
                                         color=ACCENT, lw=0.8, zorder=7, shrinkA=0, shrinkB=0))
        lab = 'start' if R['it'] == 0 else 'pass %d' % R['it']
        if R['it'] == 0:
            l2 = '$\\sum w\\delta=%.2f$' % R['wtot']
        elif R['wupd']:
            l2 = 'no gain: $w$ updated, $\\sum w\\delta$: %.2f$\\to$%.2f' % (R['wtot'], R['wnew'])
        else:
            l2 = '%d moved, $\\sum w\\delta=%.2f$' % (len(R['moved']), R['wtot'])
        ax.set_title('%s (%s)\n%s' % (lab, '$\\sum\\delta=%.2f$' % R['tot'], l2), fontsize=6.3, color=INK)
    fig.tight_layout(pad=0.2, w_pad=0.2, h_pad=0.4)
    fig.savefig(os.path.join(OUT, 'passes.pdf'))
    fig.savefig(os.path.join(OUT, 'passes.png'), dpi=200)
    print('pasadas:', len(rec), ['%d:%s%.2f' % (R['it'], 'W' if R['wupd'] else '', R['tot']) for R in rec])


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    pieces, ty, W, t, H = fig_construction()
    fig_descend()
    fig_nfp(pieces)
    fig_axis(pieces)
    fig_overlap(pieces, ty, W, t, H)
    fig_passes(pieces, ty, W, t, H)
