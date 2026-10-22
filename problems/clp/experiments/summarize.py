#!/usr/bin/env python3
"""
Resumen de varias corridas (directorios de run_uld.sh) de una misma configuracion:
media y desviacion estandar de los ULDs, cajas no cargadas, violaciones del CoG, tiempo
y comparacion por instancia con Hessler et al. (2024).

uso: summarize.py --table 8|10|12 <dir1> [dir2 ...]
"""
import sys, re, glob, os, json, statistics as st

args = sys.argv[1:]
table = '8'
if '--table' in args:
    i = args.index('--table'); table = args[i + 1]; del args[i:i + 2]
root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
H = json.load(open(os.path.join(root, 'benchs', 'ULD', f'hessler2024_table{table}.json')))

def n_items(b): return int(b[1:b.index('s')])

rows = []
for d in args:
    C, L, T = {}, {}, {}
    V = VZ = feas = 0
    for f in glob.glob(d + '/*.val'):
        b = os.path.basename(f)[:-4]
        m = re.search(r"feasible (\d) n_ulds (\d+) (?:unloaded (\d+) )?cost [\d.]+ cog_violations (\d+) (?:cog_z_violations (\d+) )?utilizations", open(f).read())
        if not m: continue
        feas += int(m.group(1)); C[b] = int(m.group(2)); L[b] = n_items(b) - int(m.group(3) or 0)
        V += int(m.group(4)); VZ += int(m.group(5) or 0)
        T[b] = float(re.search(r"time ([\d.]+)", open(f[:-4] + '.out').read()).group(1))
    k = sorted(set(C) & set(H))
    hl = {x: H[x].get('L', n_items(x)) for x in k}
    better = sum(L[x] > hl[x] or (L[x] == hl[x] and C[x] < H[x]['C']) for x in k)
    worse = sum(L[x] < hl[x] or (L[x] == hl[x] and C[x] > H[x]['C']) for x in k)
    rows.append(dict(n=len(k), feas=feas, ulds=sum(C[x] for x in k), unl=sum(n_items(x) - L[x] for x in k),
                     cog=V, cogz=VZ, better=better, worse=worse, t=st.mean(T.values()), tmax=max(T.values())))

def ms(key, fmt='{:.1f}'):
    v = [r[key] for r in rows]
    return (fmt.format(st.mean(v)) + (' ± ' + fmt.format(st.stdev(v)) if len(v) > 1 else ''))

hk = sorted(H)
print(f"corridas {len(rows)}  instancias {rows[0]['n']}  factibles {min(r['feas'] for r in rows)}/{rows[0]['n']}")
print(f"ULDs {ms('ulds')} (Hessler {sum(H[x]['C'] for x in hk)})  cajas no cargadas {ms('unl')} "
      f"(Hessler {sum(n_items(x) - H[x].get('L', n_items(x)) for x in hk)})")
print(f"mejor {ms('better')}  peor {ms('worse')}  CoG xy {ms('cog')}  CoG z {ms('cogz')}  "
      f"tiempo medio {ms('t', '{:.2f}')} s  max {max(r['tmax'] for r in rows):.2f} s")
