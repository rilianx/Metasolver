#!/usr/bin/env python3
"""Resumen de CSVs de run_bpp.sh: bins totales por clase (y gap a la cota) para cada archivo."""
import csv, sys
from collections import defaultdict
rows = {}
for f in sys.argv[1:]:
    d = defaultdict(lambda: [0, 0, 0])
    for r in csv.DictReader(open(f)):
        k = int(r["class"])
        d[k][0] += int(r["bins"]); d[k][1] += int(r["lb"]); d[k][2] += 1
    rows[f] = d
classes = sorted({k for d in rows.values() for k in d})
print("%-40s" % "config" + "".join("%9s" % f"c{k}" for k in classes) + "%9s%9s" % ("total", "gap"))
for f, d in rows.items():
    tot = sum(d[k][0] for k in classes); lb = sum(d[k][1] for k in classes)
    print("%-40s" % f.split("/")[-1] + "".join("%9d" % d[k][0] for k in classes) + "%9d%9d" % (tot, tot - lb))
