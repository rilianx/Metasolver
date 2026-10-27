#!/usr/bin/env python3
"""
Resumen de los CSV de run_mpv.sh: suma de contenedores por clase y n (10 instancias), la cota
L2 (primer numero del encabezado de cada instancia en los archivos originales) y el segundo
numero del encabezado (H), y las soluciones invalidas.
uso: summarize_mpv.py <csv> [<csv> ...]
"""
import csv, os, sys
from collections import defaultdict
root = os.path.join(os.path.dirname(__file__), "..", "benchs", "MPV", "original")

def headers(c, n):
    rows = [l.split() for l in open(os.path.join(root, f"class{c}", f"{n}.txt")) if l.strip()]
    res, r = {}, 0
    while r < len(rows):
        k, a, b = map(int, rows[r]); res[k] = (a, b); r += 2 + int(rows[r + 1][0])
    return res

data = {f: {r["instance"]: r for r in csv.DictReader(open(f))} for f in sys.argv[1:]}
keys = sorted({(int(r["class"]), int(r["n"])) for d in data.values() for r in d.values()})
print("%6s %5s %6s %6s" % ("class", "n", "LB", "H") + "".join("%14s" % os.path.basename(f)[:13] for f in data))
tot = defaultdict(int)
for c, n in keys:
    h = headers(c, n)
    lb = sum(a for a, b in h.values()); hb = sum(b for a, b in h.values())
    tot["LB"] += lb; tot["H"] += hb
    line = "%6d %5d %6d %6d" % (c, n, lb, hb)
    for f, d in data.items():
        s = sum(int(r["bins"]) for k, r in d.items() if r["class"] == str(c) and r["n"] == str(n))
        tot[f] += s
        line += "%14d" % s
    print(line)
print("%6s %5s %6d %6d" % ("total", "", tot["LB"], tot["H"]) + "".join("%14d" % tot[f] for f in data))
for f, d in data.items():
    bad = [k for k, r in d.items() if r["valid"].strip() != r["bins"].strip()]
    lbne = [k for k, r in d.items() if int(r["lb"]) != headers(int(r["class"]), int(r["n"]))[int(k.split("_")[2])][0]]
    print(os.path.basename(f), "instances", len(d), "invalid", len(bad), bad[:5], "lb != header", len(lbne))
