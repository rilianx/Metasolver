"""
Figuras del paper (requiere numpy, scipy, matplotlib).

uso (desde problems/plp/paper): python make_figures.py
  figures/merged_time.pdf     utilizacion media en el tiempo (Merged1-5) frente a 3DNest
  figures/solutions.pdf       mejores soluciones de Stoyan 2005 Example3 y Merged5
"""
import csv, json, os, sys
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from scipy.spatial import ConvexHull

HERE = os.path.dirname(os.path.abspath(__file__))
BENCHS = os.path.join(HERE, '..', 'benchs')
SOLS = os.path.join(HERE, '..', 'results', 'solutions_final')
OUT = os.path.join(HERE, 'figures')

OURS, OTHER = '#2a78d6', '#eb6834'
INK, MUTED = '#2b2b2a', '#8a8984'
plt.rcParams.update({'font.size': 8, 'axes.edgecolor': MUTED, 'axes.labelcolor': INK, 'xtick.color': INK,
                     'ytick.color': INK, 'axes.spines.top': False, 'axes.spines.right': False,
                     'font.family': 'serif'})

# volumen de Merged1 (15 piezas) y utilizacion media de 3DNest (Egeblad et al. 2009, Tabla 3)
V1 = 1687.3255
EGEBLAD = {1: (23.44, 37.29, 39.68, 42.38, 42.97), 2: (23.62, 30.23, 39.77, 42.80, 42.92),
           3: (24.58, 27.02, 35.49, 42.23, 43.32), 4: (24.80, 26.09, 31.61, 40.06, 41.99),
           5: (26.17, 26.66, 29.63, 37.99, 40.96)}
EG_T = (0.1, 10, 60, 300, 600)   # el valor "0 s" se dibuja en 0.1 s (escala logaritmica)
CPU = 2.4                        # rendimiento por nucleo: Xeon E5-2620 v4 / Core Duo T2600 (PassMark)


def merged_time():
    traj = {}
    for r in csv.DictReader(open(os.path.join(HERE, 'data', 'merged_trajectories.csv'))):
        traj.setdefault(int(r['instance'][6:]), {}).setdefault(int(r['seed']), []).append((float(r['time_s']), float(r['height'])))
    grid = np.logspace(-1, np.log10(120), 200)
    fig, axs = plt.subplots(1, 5, figsize=(7.2, 2.1), sharey=True)
    for k, i in enumerate(range(1, 6)):
        W = json.load(open(os.path.join(BENCHS, 'Egeblad et al. 2009', 'Merged%d.json' % i)))['container']['size-x']
        V = i*V1
        us = []
        for seed, pts in traj[i].items():
            u = []
            for t in grid:
                hs = [h for (s, h) in pts if s <= t]
                u.append(100*V/(W*W*min(hs)) if hs else np.nan)
            us.append(u)
        ax = axs[k]
        ax.plot(grid, np.mean(us, axis=0), color=OURS, lw=1.5, label='This work (mean of 10, 120 s)')
        ax.plot([EG_T[0]] + [x/CPU for x in EG_T[1:]], EGEBLAD[i], color=OTHER, lw=1.2, ls='--', marker='o', ms=3.5,
                label='3DNest (mean of 10), times divided by %.1f' % CPU)
        ax.set_xscale('log')
        ax.set_xticks([0.1, 1, 10, 100, 250]); ax.set_xticklabels(['0', '1', '10', '100', '250'])
        ax.axvline(600/CPU, color=MUTED, lw=0.5, ls=':')
        ax.set_title('Merged%d (%d items)' % (i, 15*i), fontsize=8, color=INK)
        ax.grid(axis='y', color='#e4e3df', lw=0.6)
        ax.set_xlabel('time (s), our machine')
        ax.set_xlim(0.08, 320)
        if k == 0: ax.set_ylabel('utilization (%)')
    axs[0].set_ylim(20, 52)
    h, l = axs[0].get_legend_handles_labels()
    fig.legend(h, l, loc='upper center', ncol=2, frameon=False, fontsize=7.5)
    fig.tight_layout(pad=0.4, w_pad=0.6, rect=(0, 0, 1, 0.9))
    fig.savefig(os.path.join(OUT, 'merged_time.pdf'))


# --- piezas y soluciones (mismo criterio que tools/check_solution.py) ---
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


def shells(F):
    out, cnt, start, open_ = [], {}, 0, 0
    for j, f in enumerate(F):
        for k in range(len(f)):
            e = tuple(sorted((f[k], f[(k+1) % len(f)])))
            c = cnt.get(e, 0) + 1; cnt[e] = c
            open_ += 1 if c == 1 else -1 if c == 2 else 0
        if open_ == 0: out.append((start, j+1)); start = j+1; cnt = {}
    return out


def read_stl_vertices(path):
    data = open(path, 'rb').read()
    n = int.from_bytes(data[80:84], 'little') if len(data) >= 84 else -1
    if n >= 0 and len(data) == 84 + 50*n:   # binario
        rec = np.frombuffer(data[84:], dtype=np.dtype([('n', '<f4', 3), ('v', '<f4', (3, 3)), ('a', '<u2')]), count=n)
        return rec['v'].reshape(-1, 3).astype(float)
    return np.array([[float(x) for x in l.split()[1:4]] for l in data.decode().splitlines() if l.strip().startswith('vertex')])


def load_parts(rel):
    dec = os.path.join(BENCHS, 'decompositions', rel.rsplit('.', 1)[0] + '.obj')
    if os.path.exists(dec):
        V, F, groups = read_obj(dec)
        rng = [(groups[k], groups[k+1] if k+1 < len(groups) else len(F)) for k in range(len(groups))] if len(groups) > 1 else shells(F)
        parts = [V[sorted(set(i for f in F[a:b] for i in f))] for a, b in rng]
    elif rel.lower().endswith('.stl'):
        parts = [read_stl_vertices(os.path.join(BENCHS, rel))]
    else:
        parts = [read_obj(os.path.join(BENCHS, rel))[0]]
    lo = np.min([P.min(axis=0) for P in parts], axis=0)
    return [P - lo for P in parts]


def draw(ax, inst, sol, title):
    I = json.load(open(os.path.join(BENCHS, inst))); S = json.load(open(os.path.join(SOLS, sol)))
    W, D = I['container']['size-x'], I['container']['size-y']
    pieces = {it['path'].split('/')[-1]: load_parts(it['path']) for it in I['item-types']}
    names = sorted(pieces)
    cmap = plt.get_cmap('tab20')
    H = 0
    for it in S['items']:
        M = np.array(it['transformation']['matrix'])
        col = cmap(names.index(it['name']) % 20)
        for P in pieces[it['name']]:
            Q = (np.hstack([P, np.ones((len(P), 1))]) @ M)[:, :3]
            H = max(H, Q[:, 2].max())
            h = ConvexHull(Q)
            ax.add_collection3d(Poly3DCollection([Q[s] for s in h.simplices], facecolor=col, edgecolor=(0, 0, 0, 0.15),
                                                 linewidths=0.2, alpha=0.95))
    ax.set_xlim(0, W); ax.set_ylim(0, D); ax.set_zlim(0, H)
    ax.set_box_aspect((W, D, H))
    ax.view_init(elev=18, azim=-60)
    ax.set_axis_off()
    ax.text2D(0.5, 0.98, '%s, height %.2f' % (title, H), transform=ax.transAxes, ha='center', va='top', fontsize=8, color=INK)


def solutions():
    fig = plt.figure(figsize=(7.2, 3.6))
    ax1 = fig.add_subplot(1, 2, 1, projection='3d')
    draw(ax1, 'Stoyan et al. 2005/Example3.json', 'Stoyan2005_Example3_27.05.json', 'Stoyan 2005 Example 3')
    ax2 = fig.add_subplot(1, 2, 2, projection='3d')
    draw(ax2, 'Egeblad et al. 2009/Merged5.json', 'Egeblad2009_Merged5_27.07.json', 'Merged5')
    fig.subplots_adjust(left=0, right=1, bottom=0, top=1, wspace=0)
    fig.savefig(os.path.join(OUT, 'solutions.pdf'))


if __name__ == '__main__':
    os.makedirs(OUT, exist_ok=True)
    merged_time()
    solutions()
