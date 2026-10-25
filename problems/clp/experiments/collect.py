#!/usr/bin/env python3
"""
Datos de las figuras del paper a partir de los resultados de run_all.sh.

uso: collect.py <resultados> <directorio de salida>
escribe time.csv (curva de ULDs segun el tiempo, escenario paquay) y tradeoff.csv (ULDs y
violaciones del CoG segun cog_penalty, en los tres escenarios).
"""
import sys, os, re, glob, statistics as st

DEFAULT_LAMBDA = 2.9119

def run(d):
    """ULDs, cajas no cargadas, ULDs con el CoG fuera de rango (xy, z) y tiempo medio de una corrida"""
    C = L = V = VZ = 0
    T = []
    for f in glob.glob(d + '/*.val'):
        b = os.path.basename(f)[:-4]
        m = re.search(r"n_ulds (\d+) (?:unloaded (\d+) )?cost [\d.]+ cog_violations (\d+) (?:cog_z_violations (\d+) )?", open(f).read())
        if not m: continue
        C += int(m.group(1)); L += int(m.group(2) or 0); V += int(m.group(3)); VZ += int(m.group(4) or 0)
        T.append(float(re.search(r"time ([\d.]+)", open(f[:-4] + '.out').read()).group(1)))
    return dict(ulds=C, unloaded=L, cog_xy=V, cog_z=VZ, time=st.mean(T))

def mean_runs(dirs):
    rs = [run(d) for d in dirs]
    return {k: st.mean(r[k] for r in rs) for k in rs[0]}

res, out = sys.argv[1], sys.argv[2]
os.makedirs(out, exist_ok=True)

with open(os.path.join(out, 'time.csv'), 'w') as f:
    f.write('t,ulds,time\n')
    pts = {}
    for d in glob.glob(res + '/curve_t*'):
        pts[float(d.split('curve_t')[1])] = run(d)
    for t in (0.1, 1):
        dirs = sorted(glob.glob(f"{res}/final_paquay_t{t}_r*"))
        if dirs: pts[t] = mean_runs(dirs)
    for t in sorted(pts):
        f.write(f"{t},{pts[t]['ulds']:.1f},{pts[t]['time']:.3f}\n")

with open(os.path.join(out, 'tradeoff.csv'), 'w') as f:
    f.write('setting,lambda,ulds,unloaded,cog_xy,cog_z\n')
    for v in ('paquay', 'adapted', 'one'):
        pts = {}
        for d in glob.glob(f"{res}/lambda*_{v}"):
            pts[float(os.path.basename(d)[len('lambda'):-len(v) - 1])] = run(d)
        dirs = sorted(glob.glob(f"{res}/final_{v}_t0.1_r*"))
        if dirs: pts[DEFAULT_LAMBDA] = mean_runs(dirs)
        for lam in sorted(pts):
            p = pts[lam]
            f.write(f"{v},{lam},{p['ulds']:.1f},{p['unloaded']:.1f},{p['cog_xy']:.1f},{p['cog_z']:.1f}\n")
