#!/usr/bin/env python3
"""
Figura de una solucion de ejemplo: cada ULD (con sus recortes) y sus cajas en 3D.

uso: make_loading.py <instancia.csv> <uld.csv> <solucion> <salida.pdf>
(solucion: formato de BSG_ULD -o; las cajas fragiles se pintan de otro color)
"""
import sys, csv
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection, Line3DCollection

inst, uldf, solf, out = sys.argv[1:5]

items = [list(map(float, filter(None, l.strip().split(';')))) for l in open(inst) if l.strip()]
fragile = [it[7] != 0 for it in items]

ulds, VOL = {}, {}
for l in open(uldf):
	f = list(map(float, filter(None, l.strip().split(';'))))
	if len(f) < 22: continue
	L, W, H = f[0], f[1], f[2]
	cuts = []
	for k in range(4):
		a, b, c = f[8+3*k], f[9+3*k], f[10+3*k]
		if a == 0 and b == 0 and c == 0: continue
		# a*z + b*x <= c, orientado para contener el centro del ULD
		if b*L/2 + a*H/2 > c: a, b, c = -a, -b, -c
		cuts.append((b, a, c))
	ulds[int(f[21])] = (L, W, H, cuts)
	VOL[int(f[21])] = f[20]

rows = list(csv.DictReader(open(solf), delimiter=';'))
loads = {}
for r in rows:
	loads.setdefault(int(r['uld']), (int(r['uld_type']), int(r['substructure']), []))[2].append(r)

def profile(L, H, cuts):
	"""poligono del ULD en el plano x-z (rectangulo recortado por los semiplanos)"""
	P = [(0, 0), (L, 0), (L, H), (0, H)]
	for nx, nz, c in cuts:
		Q = []
		for i in range(len(P)):
			p, q = P[i], P[(i+1) % len(P)]
			fp, fq = nx*p[0]+nz*p[1]-c, nx*q[0]+nz*q[1]-c
			if fp <= 0: Q.append(p)
			if fp*fq < 0:
				t = fp/(fp-fq)
				Q.append((p[0]+t*(q[0]-p[0]), p[1]+t*(q[1]-p[1])))
		P = Q
	return P

def cuboid(x, y, z, l, w, h):
	v = [(x, y, z), (x+l, y, z), (x+l, y+w, z), (x, y+w, z),
	     (x, y, z+h), (x+l, y, z+h), (x+l, y+w, z+h), (x, y+w, z+h)]
	F = [(0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 5, 4), (2, 3, 7, 6), (1, 2, 6, 5), (0, 3, 7, 4)]
	return [[v[i] for i in f] for f in F]

BOXES, FRAG, EDGE, OUTL = ['#9ec5ef', '#b9d5f3', '#7fb1e6', '#cfe1f6'], '#f2a07b', '#3a5a80', '#6b6a63'
plt.rcParams.update({'font.family': 'serif', 'font.size': 8, 'pdf.fonttype': 42})
n = len(loads)
fig = plt.figure(figsize=(2.6*n, 2.9))
for k, u in enumerate(sorted(loads)):
	t, sub, boxes = loads[u]
	L, W, H, cuts = ulds[t]
	ax = fig.add_subplot(1, n, k+1, projection='3d')
	P = profile(L, H, cuts)
	segs = []
	for i in range(len(P)):
		(x1, z1), (x2, z2) = P[i], P[(i+1) % len(P)]
		segs += [[(x1, 0, z1), (x2, 0, z2)], [(x1, W, z1), (x2, W, z2)], [(x1, 0, z1), (x1, W, z1)]]
	ax.add_collection3d(Line3DCollection(segs, colors=OUTL, linewidths=0.6))
	faces, colors = [], []
	vol = 0
	for j, r in enumerate(boxes):
		x, y, z, l, w, h = (float(r[c]) for c in ('x', 'y', 'z', 'l', 'w', 'h'))
		vol += l*w*h
		for f in cuboid(x, y, z, l, w, h):
			faces.append(f); colors.append(FRAG if fragile[int(r['item'])] else BOXES[j % len(BOXES)])
	ax.add_collection3d(Poly3DCollection(faces, facecolors=colors, edgecolors=EDGE, linewidths=0.25, alpha=0.9))
	ax.set_xlim(0, L); ax.set_ylim(0, W); ax.set_zlim(0, H)
	ax.set_box_aspect((L, W, H))
	ax.view_init(elev=22, azim=-58)
	ax.set_axis_off()
	ax.set_title(f'type {t}{" (substructure)" if sub else ""}: {len(boxes)} boxes, {100*vol/VOL[t]:.0f}%', fontsize=8, pad=-4)
fig.subplots_adjust(left=0, right=1, bottom=0, top=0.95, wspace=0)
fig.savefig(out, bbox_inches='tight', pad_inches=0.02)
