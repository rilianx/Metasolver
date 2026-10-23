# Filas de las tablas por tamano del paper (tab:size y tab:size2). uso: size_tables.py <directorio>
import re, json, glob, statistics as st, sys
S=sys.argv[1]  # directorio con hessler_ulds.json, lb_old.txt (lower_bound_uld.py) y runs/ (results/server/runs.tar.gz)
H=json.load(open(f'{S}/hessler_ulds.json'))
H12=json.load(open('problems/clp/benchs/ULD/hessler2024_table12.json'))
lb={re.search(r'(n\d+sample\d+)',l).group(1):int(l.split()[2]) for l in open(f'{S}/lb_old.txt')}
P18C=[47,54,72,83,90,108,126,129,141,160]; P18U=[33.2,34.6,36.6,37.9,42.2,41.5,45.7,47.0,47.5,50.2]
GV=[0,3,6,4,5,10,6,12,13,20]
def size(n): return int(n[1:n.index('s')])
cache={}
def ours(s,t):
    if (s,t) in cache: return cache[s,t]
    runs=[]
    for d in sorted(glob.glob(f'{S}/runs/final_{s}_t{t}_r*')):
        R={}
        for f in glob.glob(d+'/*.out'):
            n=f.split('/')[-1][:-4]; txt=open(f).read()
            ul=[float(m.group(1)) for m in re.finditer(r'utilization ([\d.]+)%',txt) if 'ULD' in txt]
            ul=[float(m.group(2)) for m in re.finditer(r'ULD \d+ type (\d+)[^:]*: boxes \d+ utilization ([\d.]+)%',txt)]
            un=int(re.search(r'unloaded (\d+)',txt).group(1))
            R[n]=(ul,un)
        runs.append(R)
    cache[s,t]=runs; return runs
def f1(x): return f'{x:.1f}'
def nU(runs,ns): return st.mean(sum(len(R[n][0]) for n in ns) for R in runs)
def med(runs,ns): return st.median([u for R in runs for n in ns for u in R[n][0]])
def unl(runs,ns): return st.mean(sum(R[n][1] for n in ns) for R in runs)
def g(x): return f'{x:g}' if x==int(x) else f1(x)
L1=[];L2=[]
tot=[0]*12
for i,k in enumerate(range(10,101,10)):
    h=H['9']; ns=[n for n in h if size(n)==k]
    r1,r2=ours('paquay','0.1'),ours('paquay','1')
    L1.append(f"{k} & {sum(lb[n] for n in ns)} & {P18C[i]} & {f1(P18U[i])} & {sum(len(h[n]) for n in ns)} & {f1(st.median([u for n in ns for _,u in h[n]]))} & {GV[i]} & {g(nU(r1,ns))} & {f1(med(r1,ns))} & {g(nU(r2,ns))} & {f1(med(r2,ns))} \\\\")
    ha=H['11']; a1,a2=ours('adapted','0.1'),ours('adapted','1')
    ho=H['13']; o1,o2=ours('one','0.1'),ours('one','1')
    ru=sum(size(n)-H12[n]['L'] for n in ns)
    L2.append(f"{k} & {sum(len(ha[n]) for n in ns)} & {g(nU(a1,ns))} & {g(nU(a2,ns))} & {sum(len(ho[n]) for n in ns)} ({ru}) & {g(nU(o1,ns))} ({g(unl(o1,ns))}) & {g(nU(o2,ns))} ({g(unl(o2,ns))}) \\\\")
print('\n'.join(L1)); print(); print('\n'.join(L2))
