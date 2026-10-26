#!/usr/bin/env python3
"""
Figura del paper a partir de time.csv (ver problems/clp/experiments/collect.py).

uso: make_figures.py [directorio]   (por defecto, el directorio de este script)
escribe time.pdf
"""
import sys, os, csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.ticker

D = sys.argv[1] if len(sys.argv) > 1 else os.path.dirname(os.path.abspath(__file__))

# Hessler et al. (2025), apendice: volumen de los ULDs (m3) y tiempo medio por instancia
# (Apple M2 Pro)
HESSLER = {'paquay': (18647.5, 0, 0, 0.19), 'adapted': (16842.9, 0, 0, 0.42), 'one': (16165.6, 0, 0, 0.41)}

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

# --- volumen de los ULDs segun el tiempo (paquay) ---
rows = read('time.csv')
fig, ax = plt.subplots(figsize=(4.6, 3.0))
for imp, style, name in (('0.1', dict(ls='-', marker='o', mfc=OURS), 'BSG, improvement at 0.1$t$'),
                         ('1', dict(ls=':', marker='o', mfc='white'), 'BSG, improvement at $t$')):
	pts = [r for r in rows if r['improve'] == imp]
	t = [float(r['t']) for r in pts]; v = [float(r['vol']) for r in pts]; s = [float(r['time']) for r in pts]
	ax.plot(s, v, color=OURS, lw=1.5, ms=5, mec=OURS if imp == '1' else 'white', mew=1, label=name, zorder=3, **style)
	if imp == '0.1':
		for ti, si, vi in zip(t, s, v):
			label(ax, si, vi, f'{ti:g} s', dx=5, dy=5)
h = HESSLER['paquay']
SPEED = 4105 / 1624   # tiempo del RGS escalado a nuestra maquina (PassMark single thread)
ax.axhline(h[0], color=THEIRS, lw=0.8, ls='--', zorder=1)
ax.plot([h[3]], [h[0]], 's', color=THEIRS, ms=7, mec='white', mew=1, label='RGS (Heßler et al. 2025)', zorder=3)
ax.plot([h[3]*SPEED], [h[0]], 's', mfc='white', mec=THEIRS, mew=1.4, ms=7, label='RGS, time scaled to our machine', zorder=3)
ax.annotate('', xy=(h[3]*SPEED*0.92, h[0]), xytext=(h[3]*1.09, h[0]), arrowprops=dict(arrowstyle='->', color=THEIRS, lw=0.9))
label(ax, h[3], h[0], f'RGS: {h[0]:,.0f} m$^3$', dx=-2, dy=-12)
ax.set_xscale('log')
ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, _: f'{x:g}'))
ax.yaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda y, _: f'{y:,.0f}'))
ax.set_xlabel('mean time per instance (s, log scale)')
ax.set_ylabel('volume of the ULDs (m$^3$, 300 instances)')
ax.set_ylim(17000, 18800)
ax.legend(loc='lower left', fontsize=7.5)
fig.tight_layout()
fig.savefig(os.path.join(D, 'time.pdf'))
