#!/usr/bin/env python3
"""
Figura esquematica de una solucion como liquido: cada ULD con su perfil x-z (con los recortes) y
lleno hasta la altura que deja su utilizacion como proporcion del area del perfil.

uso: make_liquid.py <uld.csv> <salida.pdf> <metodo>=<tipo>:<utilizacion>,... [...]
ejemplo: make_liquid.py uld.csv liquid.pdf "RGS=5:69.6,6:76.6,4:25.2" "BSG=6:91.6,5:72.3,1:13.8"
"""
import sys
import matplotlib
matplotlib.use('Agg')
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

uldf, out = sys.argv[1:3]
sols = []
for a in sys.argv[3:]:
	name, rest = a.split('=')
	sols.append((name, [(int(t), float(u)) for t, u in (x.split(':') for x in rest.split(','))]))

ulds, VOL = {}, {}
for l in open(uldf):
	f = list(map(float, filter(None, l.strip().split(';'))))
	if len(f) < 22: continue
	L, H = f[0], f[2]
	cuts = []
	for k in range(4):
		a, b, c = f[8+3*k], f[9+3*k], f[10+3*k]
		if a == 0 and b == 0 and c == 0: continue
		if b*L/2 + a*H/2 > c: a, b, c = -a, -b, -c
		cuts.append((b, a, c))
	ulds[int(f[21])] = (L, H, cuts)
	VOL[int(f[21])] = f[20] * 1e-9

def clip(P, nx, nz, c):
	"""parte del poligono P con nx*x + nz*z <= c"""
	Q = []
	for i in range(len(P)):
		p, q = P[i], P[(i+1) % len(P)]
		fp, fq = nx*p[0]+nz*p[1]-c, nx*q[0]+nz*q[1]-c
		if fp <= 0: Q.append(p)
		if fp*fq < 0:
			t = fp/(fp-fq)
			Q.append((p[0]+t*(q[0]-p[0]), p[1]+t*(q[1]-p[1])))
	return Q

def area(P):
	return abs(sum(P[i][0]*P[(i+1) % len(P)][1] - P[(i+1) % len(P)][0]*P[i][1] for i in range(len(P)))) / 2

def profile(t):
	L, H, cuts = ulds[t]
	P = [(0, 0), (L, 0), (L, H), (0, H)]
	for nx, nz, c in cuts: P = clip(P, nx, nz, c)
	return P

def liquid(P, u):
	"""poligono del liquido: la parte de P bajo la altura que deja la proporcion u del area"""
	A, lo, hi = area(P), 0.0, max(z for x, z in P)
	for _ in range(60):
		h = (lo+hi)/2
		if area(clip(P, 0, 1, h)) < u*A: lo = h
		else: hi = h
	return clip(P, 0, 1, (lo+hi)/2)

FILL, OUTL = '#7fb1e6', '#3a5a80'
GAP, SEP = 250, 900
fig, ax = plt.subplots(figsize=(7, 1.5))
x0, Hmax = 0, max(max(ulds[t][1] for t, u in s) for n, s in sols)
for g, (name, sol) in enumerate(sols):
	start = x0
	for t, u in sol:
		P = [(x+x0, z) for x, z in profile(t)]
		ax.add_patch(Polygon(liquid(P, u/100), closed=True, facecolor=FILL, edgecolor='none'))
		ax.add_patch(Polygon(P, closed=True, fill=False, edgecolor=OUTL, linewidth=0.8))
		ax.text(x0 + ulds[t][0]/2, -80, f'type {t}, {VOL[t]:.1f} m$^3$\n{u:.1f}%', ha='center', va='top', fontsize=6.5)
		x0 += ulds[t][0] + GAP
	tot = sum(VOL[t] for t, u in sol)
	ax.text((start + x0 - GAP)/2, Hmax + 80, f'{name}: {tot:.1f} m$^3$', ha='center', va='bottom', fontsize=8)
	if g < len(sols)-1:
		ax.plot([x0 - GAP + SEP/2]*2, [-500, Hmax + 250], color='#bbbbbb', linewidth=0.6)
		x0 += SEP - GAP
ax.set_xlim(-50, x0 - GAP + 50)
ax.set_ylim(-520, Hmax + 330)
ax.set_aspect('equal')
ax.axis('off')
plt.savefig(out, bbox_inches='tight')
