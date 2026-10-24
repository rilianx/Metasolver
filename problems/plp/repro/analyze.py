"""
Tablas del paper a partir de los logs de run_experiments.sh (y de los certificados de analyze.sh).
uso: analyze.py carpeta_runs > results.md      (escribe ademas carpeta_runs/../merged_trajectories.csv)
"""
import re, glob, os, sys, json, csv, statistics as st

D = sys.argv[1]
HERE = os.path.dirname(os.path.abspath(__file__))
BENCHS = os.path.join(HERE, '..', 'benchs')


def parse(f):
    t = open(f).read()
    m = re.search(r'^height: (\S+)', t, re.M)
    if not m: return None
    pts = []
    m0 = re.search(r'^construction_height \(\w+, (\S+) s\): (\S+)', t, re.M)
    mc = re.search(r'^compaction_height \(.*\): (\S+)', t, re.M)
    if m0: pts.append((float(m0.group(1)), float(mc.group(1) if mc else m0.group(2))))
    for m2 in re.finditer(r'^\[separacion\] \((\S+) s\): altura (\S+)', t, re.M):
        pts.append((float(m2.group(1)), float(m2.group(2))))
    return {'pts': pts, 'h': float(m.group(1)), 'feas': 'feasible: yes' in t, 'file': f,
            'seed': int(re.search(r'\.s(\d+)\.', os.path.basename(f)).group(1))}


def runs(cfg, key):
    return [r for r in (parse(f) for f in sorted(glob.glob('%s/%s.s*.%s.log' % (D, cfg, key)))) if r]


def h_at(r, T):
    hs = [h for (s, h) in r['pts'] if s <= T]
    return min(hs) if hs else float('nan')


def cert(r):
    c = r['file'][:-4] + '.cert'
    if not os.path.exists(c): return None
    return 'certified: yes' in open(c).read()


def key(inst): return inst.replace('/', '__').replace(' ', '_')


S04 = ['Stoyan et al. 2004/Example%d' % i for i in range(1, 6)]
S05 = ['Stoyan et al. 2005/Example%d' % i for i in range(1, 4)]
MER = ['Egeblad et al. 2009/Merged%d' % i for i in range(1, 6)]
# referencias de la tabla del paper (Lamas-Fernandez et al. 2023, mejor altura publicada con geometria exacta o voxeles)
REF = {'Stoyan et al. 2004/Example2': 28.47, 'Stoyan et al. 2004/Example3': 41.30, 'Stoyan et al. 2004/Example4': 54.83,
       'Stoyan et al. 2004/Example5': 72.33, 'Stoyan et al. 2005/Example2': 19.01, 'Stoyan et al. 2005/Example3': 28.83}
HARD = S04 + S05[1:]

print('# Resultados (%s)\n' % D)
hw = os.path.join(D, '..', 'hardware.txt')
if os.path.exists(hw):
    t = open(hw).read()
    for k in ('Model name', 'Socket\\(s\\)', 'Core\\(s\\) per socket', 'Thread\\(s\\) per core', 'CPU max MHz'):
        m = re.search(r'^%s:\s*(.*)$' % k, t, re.M)
        if m: print('- %s: %s' % (k.replace('\\', ''), m.group(1)))
    print()

print('## Tabla principal (120 s, 10 semillas)\n')
print('| Instancia | n | Media | Mejor | Desv. | Infactibles | Certificadas |')
print('|---|---|---|---|---|---|---|')
ncert = ntot = 0
for inst in S05 + S04 + MER + ['Liu et al. 2015/Experiment1']:
    R = runs('full', key(inst))
    if not R: continue
    hs = [r['h'] for r in R]; c = [cert(r) for r in R]
    ncert += sum(x is True for x in c); ntot += sum(x is not None for x in c)
    print('| %s | %d | %.2f | %.2f | %.2f | %d | %s |' % (inst, len(R), st.mean(hs), min(hs), st.pstdev(hs),
          sum(not r['feas'] for r in R), '%d/%d' % (sum(x is True for x in c), sum(x is not None for x in c))))
print('\nCertificadas en total: %d de %d\n' % (ncert, ntot))

print('## Ablacion (brecha media respecto de la referencia; Stoyan 2004 Ex1: altura media)\n')
print('| Variante | Stoyan 2004 | Stoyan 2005 | S2004 Ex1 | n |')
print('|---|---|---|---|---|')
for name, cfg, T in (('Propuesto', 'full', None), ('Sin LP', 'nolp', None), ('Pesos aditivos', 'add', None),
                     ('Solo reinsercion', 'reinsert', None), ('5x mas lento (24 s)', 'full', 24)):
    g = {'2004': [], '2005': []}; ok = True
    for inst, ref in REF.items():
        R = runs(cfg, key(inst))
        if not R: ok = False; break
        hs = [h_at(r, T) if T else r['h'] for r in R]
        g['2004' if '2004' in inst else '2005'].append((st.mean(hs)-ref)/ref*100)
    R = runs(cfg, key(S04[0]))
    if not ok or not R: continue
    e1 = st.mean([h_at(r, T) if T else r['h'] for r in R])
    print('| %s | %+.1f%% | %+.1f%% | %.2f | %d |' % (name, st.mean(g['2004']), st.mean(g['2005']), e1, len(R)))

print('\n## Merged: utilizacion media en el tiempo (3DNest a 600 s: 43.0, 42.9, 43.3, 42.0, 41.0 %)\n')
V1 = 1687.3255
print('| Instancia | 1 s | 10 s | 60 s | 120 s |')
print('|---|---|---|---|---|')
traj = []
for i, inst in enumerate(MER, 1):
    R = runs('full', key(inst))
    if not R: continue
    W = json.load(open(os.path.join(BENCHS, inst + '.json')))['container']['size-x']
    u = lambda h: 100*i*V1/(W*W*h)
    print('| Merged%d | %s |' % (i, ' | '.join('%.1f' % st.mean(u(h_at(r, T)) for r in R) for T in (1, 10, 60, 120))))
    for r in R:
        best = 1e9
        for t, h in sorted(r['pts']):
            if h < best: best = h; traj.append('Merged%d,%d,%.3f,%.6f' % (i, r['seed'], t, h))
with open(os.path.join(D, '..', 'merged_trajectories.csv'), 'w') as f:
    f.write('instance,seed,time_s,height\n' + '\n'.join(traj) + '\n')

ref = {}
for row in csv.DictReader(open(os.path.join(HERE, 'data', 'tollenaere2025_reference.csv'))):
    ref[(row['series'], int(row['items']))] = (float(row['best_ub']), float(row['best_lb']), row['proven_optimal'] == '1')
T25 = {}
for f in glob.glob(D + '/full.s*.Tollenaere*.log'):
    r = parse(f)
    if not r: continue
    m = re.search(r'Instances__(\w+)__(\d+)_ITEMS', f)
    T25.setdefault((m.group(1).replace('_', ' '), int(m.group(2))), []).append(r)
if T25:
    print('\n## Tollenaere et al. 2025 (30 s, 10 semillas)\n')
    print('| Serie | Instancias | Optimos (T2025) | Optimos alcanzados | Abiertas | Abiertas mejoradas | Brecha media (media) | Brecha media (mejor) | Infactibles |')
    print('|---|---|---|---|---|---|---|---|---|')
    tot = [0]*6
    for ser in sorted({k[0] for k in T25}):
        ks = sorted(k for k in T25 if k[0] == ser)
        n = len(ks); nopt = reach = nopen = impr = inf = 0; gm = []; gb = []
        for k in ks:
            ub, lb, opt = ref[k]; hs = [r['h'] for r in T25[k]]; b = min(hs)
            gm.append((st.mean(hs)-ub)/ub*100); gb.append((b-ub)/ub*100); inf += sum(not r['feas'] for r in T25[k])
            if opt: nopt += 1; reach += b <= ub + 0.005
            else: nopen += 1; impr += b < ub - 0.005
        for j, v in enumerate((n, nopt, reach, nopen, impr, inf)): tot[j] += v
        print('| %s | %d | %d | %d | %d | %d | %+.2f%% | %+.2f%% | %d |' % (ser, n, nopt, reach, nopen, impr, st.mean(gm), st.mean(gb), inf))
    print('| Total | %d | %d | %d | %d | %d | | | %d |' % tuple(tot))

ch = [(name, runs(cfg, key('Lamas-Fernandez, C. et al. 2022/Chess'))) for name, cfg in
      (('fina (t=0.05), 1200 s', 'chess_fine'), ('gruesa (t=0.2, k-DOP), 1200 s', 'chess_coarse'),
       ('gruesa 900 s + fina 300 s', 'chess_mr'))]
if any(R for _, R in ch):
    print('\n## Chess (Lamas: mejor 131, media 135.1)\n')
    print('| Descomposicion | n | Media | Mejor | Infactibles |')
    print('|---|---|---|---|---|')
    for name, R in ch:
        if R:
            hs = [r['h'] for r in R]
            print('| %s | %d | %.2f | %.2f | %d |' % (name, len(R), st.mean(hs), min(hs), sum(not r['feas'] for r in R)))
