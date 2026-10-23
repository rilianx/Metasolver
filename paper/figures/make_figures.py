#!/usr/bin/env python3
"""
Figuras del paper a partir de time.csv y tradeoff.csv (ver problems/clp/experiments/collect.py).

uso: make_figures.py [directorio]   (por defecto, el directorio de este script)
escribe time.pdf y tradeoff.pdf
"""
import sys, os, csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker

D = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))

# Hessler et al. (2025), apendice: ULDs, % de ULDs con el CoG fuera de rango (x o y), cajas no
# cargadas y tiempo medio por instancia (Apple M2 Pro)
HESSLER = {'paquay': (708, 11.2, 0, 0.19), 'adapted': (653, 7.2, 0, 0.42), 'one': (729, 10.1, 24, 0.41)}
TITLES = {'paquay': 'Paquay', 'adapted': 'Adapted', 'one': 'One ULD of each type'}

OURS, THEIRS = '#2a78d6', '#eb6834'          # categorias 1 y 2 de la paleta (validada)
INK, MUTED, GRID = '#1f1f1d', '#6b6a63', '#e4e3df'

plt.rcParams.update({
	'font.family': 'serif', 'font.size': 9, 'axes.edgecolor': MUTED, 'axes.labelcolor': INK,
	'xtick.color': MUTED, 'ytick.color': MUTED, 'axes.spines.top': False, 'axes.spines.right': False,
	'axes.grid': True, 'grid.color': GRID, 'grid.linewidth': 0.6, 'axes.axisbelow': True,
	'legend.frameon': False, 'pdf.fonttype': 42,
})

def read(name):
	with open(os.path.join(D, name)) as f:
		return list(csv.DictReader(f))

def label(ax, x, y, text, dx=6, dy=4, ha='left'):
	ax.annotate(text, (x, y), xytext=(dx, dy), textcoords='offset points', fontsize=7.5, color=INK, ha=ha)

# --- ULDs segun el tiempo (paquay) ---
rows = read('time.csv')
t = [float(r['t']) for r in rows]; u = [float(r['ulds']) for r in rows]; s = [float(r['time']) for r in rows]
fig, ax = plt.subplots(figsize=(4.6, 3.0))
ax.plot(s, u, '-o', color=OURS, lw=1.5, ms=5, mec='white', mew=1, label='BSG (this work)', zorder=3)
for ti, si, ui in zip(t, s, u):
	label(ax, si, ui, f'{ti:g} s', dx=5, dy=5)
h = HESSLER['paquay']
ax.plot([h[3]], [h[0]], 's', color=THEIRS, ms=6, mec='white', mew=1, label='RGS (Heßler et al. 2025)', zorder=3)
ax.set_xscale('log')
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda v, _: f'{v:g}'))
ax.set_xlabel('mean time per instance (s, log scale)')
ax.set_ylabel('number of ULDs (300 instances)')
ax.legend(loc='upper right', fontsize=7.5)
fig.tight_layout()
fig.savefig(os.path.join(D, 'time.pdf'))

# --- ULDs vs. violaciones del CoG segun lambda ---
rows = read('tradeoff.csv')
fig, axes = plt.subplots(1, 3, figsize=(7.2, 2.7))
for ax, v in zip(axes, ('paquay', 'adapted', 'one')):
	pts = sorted([r for r in rows if r['setting'] == v], key=lambda r: float(r['lambda']))
	x = [100 * float(r['cog_xy']) / float(r['ulds']) for r in pts]
	y = [float(r['ulds']) for r in pts]
	ax.plot(x, y, 'o', color=OURS, ms=4.5, mec='white', mew=0.8, label='BSG ($\\lambda$ varies)', zorder=3)
	for r, xi, yi in zip(pts, x, y):
		lam = float(r['lambda'])
		extra = f" ({float(r['unloaded']):g})" if v == 'one' else ''
		if lam == 0.0:
			label(ax, xi, yi, f"$\\lambda$=0{extra}", dx=-4, dy=6, ha='right')
		elif abs(lam - 12.6619) < 1e-3:
			label(ax, xi, yi, f"$\\lambda$=12.7{extra}", dx=6, dy=-3)
		elif v == 'one' and lam == 0.5:
			label(ax, xi, yi, f"$\\lambda$=0.5{extra}", dx=6, dy=-8)
	h = HESSLER[v]
	ax.plot([h[1]], [h[0]], 's', color=THEIRS, ms=5.5, mec='white', mew=0.8, label='RGS', zorder=3)
	label(ax, h[1], h[0], 'RGS' + (f' ({h[2]})' if v == 'one' else ''), dx=5, dy=-10)
	ax.set_title(TITLES[v], fontsize=9, color=INK)
	ax.set_xlabel('% ULDs with CoG out of range')
axes[0].set_ylabel('number of ULDs')
axes[0].legend(loc='center right', fontsize=7.5, handletextpad=0.3)
fig.tight_layout()
fig.savefig(os.path.join(D, 'tradeoff.pdf'))
