#!/usr/bin/env python3
"""
Figura del paper (curvas de tiempo en los tres escenarios) a partir de time.csv (ver problems/clp/experiments/collect.py).

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

# --- volumen de los ULDs segun el tiempo, relativo al RGS (tres escenarios) ---
rows = read('time.csv')
SPEED = 4105 / 1624   # tiempo del RGS escalado a nuestra maquina (PassMark single thread)
TITLES = {'paquay': 'Paquay', 'adapted': 'Adapted', 'one': 'One ULD of each type'}
fig, axes = plt.subplots(1, 3, figsize=(7.4, 3.1), sharey=True)
for ax, v in zip(axes, ('paquay', 'adapted', 'one')):
	h = HESSLER[v]
	for imp, style, name in (('0.1', dict(ls='-', marker='o', mfc=OURS), 'BSG, improvement at 0.1$t$'),
	                         ('1', dict(ls=':', marker='o', mfc='white'), 'BSG, improvement at $t$')):
		pts = [r for r in rows if r['setting'] == v and r['improve'] == imp]
		if not pts: continue
		t = [float(r['t']) for r in pts]; s_ = [float(r['time']) for r in pts]
		y = [100 * (float(r['vol']) / h[0] - 1) for r in pts]
		ax.plot(s_, y, color=OURS, lw=1.4, ms=4.5, mec=OURS if imp == '1' else 'white', mew=0.8, label=name, zorder=3, **style)
		if imp == '0.1':
			for ti, si, yi in zip(t, s_, y):
				if ti in (0.02, 0.1, 1, 2.0): label(ax, si, yi, f'{ti:g} s', dx=-3, dy=-11, ha='right')
	ax.axhline(0, color=THEIRS, lw=0.8, ls='--', zorder=1)
	ax.plot([h[3]], [0], 's', color=THEIRS, ms=6, mec='white', mew=0.8, label='RGS (Heßler et al. 2025)', zorder=4)
	ax.plot([h[3]*SPEED], [0], 's', mfc='white', mec=THEIRS, mew=1.2, ms=6, label='RGS, time scaled to our machine', zorder=4)
	ax.set_xscale('log')
	ax.xaxis.set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, _: f'{x:g}'))
	ax.set_title(TITLES[v], fontsize=9, color=INK)
	ax.set_xlabel('mean time per instance (s)')
	ax.set_xlim(0.12, 40)
axes[0].set_ylabel('volume of the ULDs vs RGS (%)')
axes[0].set_ylim(-9.5, 3)
h_, l_ = axes[0].get_legend_handles_labels()
fig.legend(h_, l_, loc='lower center', ncol=2, fontsize=7.5, frameon=False, bbox_to_anchor=(0.5, 0.0))
fig.tight_layout(rect=(0, 0.14, 1, 1))
fig.savefig(os.path.join(D, 'time.pdf'))
