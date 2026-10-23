# Extrae de pdftotext -layout del arXiv 2410.01445 (Hessler et al.) las tablas 9, 11 y 13
# (tipo y utilizacion de cada ULD). uso: parse_hessler_ulds.py <texto> <salida.json>
import re, json, sys
txt=open(sys.argv[1]).read().split('\n')
starts={9:None,11:None,13:None}
idx=[i for i,l in enumerate(txt) if re.search(r'Table (9|11|13):',l)]
ends=[i for i,l in enumerate(txt) if re.search(r'Table (10|12):',l)]+[len(txt)]
crit=r'(?:s–)?[a-z]{1,2}'
pat=re.compile(r'\b(10|20|30|40|50|60|70|80|90|100)\s+(\d{1,2}?)\s*([1-6])\s+('+crit+r')\s+([\d.]+)\s+([\d.]+)\s+([\d.]+)')
V={1:5338146658,3:7919351700,5:31793619832,4:20014815825,2:9849023990,6:23198728050}
out={}
for t,s in zip((9,11,13),idx):
    e=min(x for x in ends if x>s)
    d={}
    for l in txt[s:e]:
        for m in pat.finditer(l):
            n,no,ID=m.group(1),m.group(2),int(m.group(3))
            d.setdefault(f'n{n}sample{int(no)}',[]).append((ID,float(m.group(5))))
    out[t]=d
for t,k in ((9,8),(11,10),(13,12)):
    H=json.load(open(f'/home/user/Metasolver/problems/clp/benchs/ULD/hessler2024_table{k}.json'))
    bad=[i for i in H if len(out[t].get(i,[]))!=H[i]['C']]
    print(t, len(out[t]), sum(len(v) for v in out[t].values()), sum(h['C'] for h in H.values()), 'bad', len(bad), bad[:5])
json.dump({str(k):v for k,v in out.items()}, open(sys.argv[2],'w'))
