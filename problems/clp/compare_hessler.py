#!/usr/bin/env python3
"""
Comparacion por instancia con Hessler et al. (2024), tablas del apendice:
  8:  instancias de Paquay et al. (2018b)
  10: instancias adaptadas (borde), ULDs ilimitados
  12: instancias adaptadas, un ULD de cada tipo
(benchs/ULD/hessler2024_table<k>.json: tiempo en ms, cajas cargadas, numero de ULDs y
utilizacion total).

uso: compare_hessler.py [--table 8|10|12] <dir1> [dir2 ...]
cada directorio contiene, por instancia, <instancia>.out (salida de BSG_ULD) y
<instancia>.val (salida de validate_uld.py). Una solucion es mejor si carga mas cajas o,
con las mismas cajas, usa menos ULDs. Requiere scipy (prueba de Wilcoxon).
"""
import sys, re, glob, os, json, statistics as st
from scipy.stats import wilcoxon

args = sys.argv[1:]
table = '8'
if '--table' in args:
    i = args.index('--table'); table = args[i + 1]; del args[i:i + 2]
H = json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'benchs', 'ULD', f'hessler2024_table{table}.json')))

for d in args:
    C = {}; L = {}; T = {}; V = 0; VZ = 0; feas = 0
    for f in glob.glob(d + '/*.val'):
        b = os.path.basename(f)[:-4]; s = open(f).read()
        m = re.search(r"feasible (\d) n_ulds (\d+) (?:unloaded (\d+) )?cost [\d.]+ cog_violations (\d+) (?:cog_z_violations (\d+) )?utilizations", s)
        if not m: continue
        n_items = int(b[1:b.index('s')])
        feas += int(m.group(1)); C[b] = int(m.group(2)); L[b] = n_items - int(m.group(3) or 0)
        V += int(m.group(4)); VZ += int(m.group(5) or 0)
        T[b] = float(re.search(r"time ([\d.]+)", open(f[:-4] + '.out').read()).group(1))
    k = sorted(set(C) & set(H))
    hl = {x: H[x].get('L', int(x[1:x.index('s')])) for x in k}
    better = sum(L[x] > hl[x] or (L[x] == hl[x] and C[x] < H[x]['C']) for x in k)
    worse = sum(L[x] < hl[x] or (L[x] == hl[x] and C[x] > H[x]['C']) for x in k)
    diff = [C[x] - H[x]['C'] for x in k]
    p = wilcoxon([C[x] for x in k], [H[x]['C'] for x in k]).pvalue if any(diff) else 1.0
    print(f"{os.path.basename(d)}: instancias {len(k)} factibles {feas}  ULDs {sum(C[x] for x in k)} (Hessler {sum(H[x]['C'] for x in k)})  "
          f"cajas no cargadas {sum(int(x[1:x.index('s')]) - L[x] for x in k)} (Hessler {sum(int(x[1:x.index('s')]) - hl[x] for x in k)})  "
          f"mejor {better} igual {len(k) - better - worse} peor {worse}  Wilcoxon(ULDs) p={p:.2g}  "
          f"CoG xy {V} z {VZ}  tiempo medio {st.mean(T.values()):.2f}s max {max(T.values()):.2f}s")
