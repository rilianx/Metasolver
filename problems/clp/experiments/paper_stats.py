#!/usr/bin/env python3
"""
Numeros del paper para un escenario: media (y desviacion) sobre las corridas de ULDs, cajas y
volumen sin cargar, volumen y costo de los ULDs, ULDs con el CoG fuera de rango (segun el
validador), tiempo, comparacion por instancia con Hessler et al. (mejor/peor y Wilcoxon) y,
en el escenario paquay, instancias que alcanzan la cota inferior.

uso: paper_stats.py <paquay|adapted|one> <lb.txt|-> <dir1> [dir2 ...]
  cada directorio tiene <instancia>.out (BSG_ULD -v) y <instancia>.val (validate_uld.py);
  lb.txt: salida de lower_bound_uld.py (solo se usa en paquay).
Comparacion con Hessler: con ULDs ilimitados, mejor = menos ULDs; con un ULD de cada tipo,
mejor = mas volumen cargado (diferencia de mas de 0.05 m3, por el redondeo de su apendice)
o, con el mismo volumen, menos ULDs. Requiere scipy.
"""
import sys, os, re, glob, json, statistics as st
from scipy.stats import wilcoxon

here = os.path.dirname(os.path.abspath(__file__))
bench = os.path.join(here, '..', 'benchs', 'ULD')
setting, lbfile, dirs = sys.argv[1], sys.argv[2], sys.argv[3:]
table = {'paquay': '8', 'adapted': '10', 'one': '12'}[setting]
H = json.load(open(os.path.join(bench, f'hessler2024_table{table}.json')))
HU = json.load(open(os.path.join(bench, 'hessler2024_ulds.json')))[setting]

ULD = {}
for l in open(os.path.join(bench, 'uld.csv')):
    f = list(map(float, filter(None, l.strip().split(';'))))
    if len(f) >= 22: ULD[int(f[21])] = (f[20], f[4])  # volumen, costo
TOT = {}
for f in glob.glob(os.path.join(bench, 'Final_instances', '*.csv')):
    n = os.path.basename(f)[:-4]
    TOT[n] = sum(a * b * c for a, b, c in (list(map(float, l.split(';')[:3])) for l in open(f) if l.strip()))
size = lambda n: int(n[1:n.index('s')])
TOL = 5e7  # 0.05 m3

# Hessler: volumen sin cargar, volumen y costo de los ULDs por instancia
hun = {n: (TOT[n] - sum(u['u'] / 100 * ULD[u['type']][0] for u in HU[n])) if size(n) > H[n].get('L', size(n)) else 0.0 for n in H}
hvol = sum(ULD[u['type']][0] for n in HU for u in HU[n]) * 1e-9
hcost = sum(ULD[u['type']][1] for n in HU for u in HU[n])

pat = re.compile(r'ULD \d+ type (\d+)[^:]*: boxes \d+ utilization ([\d.]+)%')
runs = []
for d in dirs:
    R = {}
    for f in glob.glob(os.path.join(d, '*.out')):
        n = os.path.basename(f)[:-4]; txt = open(f).read()
        ul = [(int(m.group(1)), float(m.group(2))) for m in pat.finditer(txt)]
        val = open(f[:-4] + '.val').read()
        m = re.search(r"feasible (\d) n_ulds (\d+) (?:unloaded (\d+) )?cost [\d.]+ cog_violations (\d+) (?:cog_z_violations (\d+) )?", val)
        un = int(m.group(3) or 0)
        unv = TOT[n] - sum(u / 100 * ULD[t][0] for t, u in ul) if un else 0.0
        R[n] = dict(ulds=len(ul), feas=int(m.group(1)), un=un, unv=unv, cog=int(m.group(4)), cogz=int(m.group(5) or 0),
                    vol=sum(ULD[t][0] for t, u in ul) * 1e-9, cost=sum(ULD[t][1] for t, u in ul),
                    time=float(re.search(r'time ([\d.]+)', txt).group(1)), util=[u for t, u in ul])
    runs.append(R)

def better(n, r):
    if setting == 'one':
        if r['unv'] < hun[n] - TOL: return 1
        if r['unv'] > hun[n] + TOL: return -1
    return (r['ulds'] < H[n]['C']) - (r['ulds'] > H[n]['C'])

def ms(vals):
    return [round(st.mean(vals), 2), round(st.pstdev(vals), 2) if len(vals) > 1 else 0.0]

res = dict(runs=len(runs), instances=len(runs[0]), feasible=min(sum(r['feas'] for r in R.values()) for R in runs))
for k in ('ulds', 'un', 'cog', 'cogz', 'cost'):
    res[k] = ms([sum(r[k] for r in R.values()) for R in runs])
res['unv'] = ms([sum(r['unv'] for r in R.values()) * 1e-9 for R in runs])
res['vol'] = ms([sum(r['vol'] for r in R.values()) for R in runs])
res['time'] = ms([st.mean(r['time'] for r in R.values()) for R in runs])
res['time_max'] = max(max(r['time'] for r in R.values()) for R in runs)
res['better'] = ms([sum(better(n, R[n]) > 0 for n in R) for R in runs])
res['worse'] = ms([sum(better(n, R[n]) < 0 for n in R) for R in runs])
ks = sorted(runs[0])
res['wilcoxon_ulds'] = [float('%.2g' % wilcoxon([R[n]['ulds'] for n in ks], [H[n]['C'] for n in ks]).pvalue) for R in runs]
res['hessler'] = dict(ulds=sum(H[n]['C'] for n in H), un=sum(size(n) - H[n].get('L', size(n)) for n in H),
                      unv=round(sum(hun.values()) * 1e-9, 2), vol=round(hvol, 1), cost=round(hcost, 1))
if setting == 'paquay' and lbfile != '-':
    lb = {re.search(r'(n\d+sample\d+)', l).group(1): int(l.split()[2]) for l in open(lbfile)}
    res['lb_total'] = sum(lb.values())
    res['optimal'] = ms([sum(R[n]['ulds'] == lb[n] for n in R) for R in runs])
    res['gap_max'] = max(R[n]['ulds'] - lb[n] for R in runs for n in R)
    res['hessler']['optimal'] = sum(H[n]['C'] == lb[n] for n in H)
# por tamano: ULDs, unloaded, mediana de la utilizacion
res['size'] = {}
for s in range(10, 101, 10):
    ns = [n for n in ks if size(n) == s]
    res['size'][s] = dict(ulds=round(st.mean(sum(R[n]['ulds'] for n in ns) for R in runs), 1),
                          un=round(st.mean(sum(R[n]['un'] for n in ns) for R in runs), 1),
                          umed=round(st.median([u for R in runs for n in ns for u in R[n]['util']]), 1),
                          hulds=sum(H[n]['C'] for n in ns), hun=sum(size(n) - H[n].get('L', size(n)) for n in ns),
                          humed=round(st.median([u['u'] for n in ns for u in HU[n]]), 1))
print(json.dumps(res, indent=1))
