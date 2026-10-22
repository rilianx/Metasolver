#!/usr/bin/env python3
"""
Comparacion por instancia con Hessler et al. (2024), Tabla 8 del apendice
(benchs/ULD/hessler2024_table8.json: tiempo en ms, numero de ULDs y utilizacion total).

uso: compare_hessler.py <dir1> [dir2 ...]
cada directorio contiene, por instancia, <instancia>.out (salida de BSG_ULD) y
<instancia>.val (salida de validate_uld.py). Requiere scipy (prueba de Wilcoxon).
"""
import sys,re,glob,os,json,statistics as st
from scipy.stats import wilcoxon
H=json.load(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'benchs','ULD','hessler2024_table8.json')))
for d in sys.argv[1:]:
    C={}; T={}; V=0; VZ=0; feas=0; ut={}
    for f in glob.glob(d+'/*.val'):
        b=os.path.basename(f)[:-4]; s=open(f).read()
        m=re.search(r"feasible (\d) n_ulds (\d+) cost [\d.]+ cog_violations (\d+) (?:cog_z_violations (\d+) )?utilizations",s)
        if not m: continue
        feas+=int(m.group(1)); C[b]=int(m.group(2)); V+=int(m.group(3)); VZ+=int(m.group(4) or 0)
        o=open(f[:-4]+'.out').read(); T[b]=float(re.search(r"time ([\d.]+)",o).group(1))
    k=sorted(set(C)&set(H)); diff=[C[x]-H[x]['C'] for x in k]
    p=wilcoxon([C[x] for x in k],[H[x]['C'] for x in k]).pvalue
    print(f"{os.path.basename(d)}: instancias {len(k)} factibles {feas}  ULDs {sum(C.values())} (Hessler {sum(H[x]['C'] for x in k)})  "
          f"mejor {sum(x<0 for x in diff)} igual {sum(x==0 for x in diff)} peor {sum(x>0 for x in diff)}  Wilcoxon p={p:.2g}  "
          f"CoG xy {V} z {VZ}  tiempo medio {st.mean(T.values()):.2f}s max {max(T.values()):.2f}s")
