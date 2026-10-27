#!/bin/bash
# Ejecuta BSG_BPP en instancias MPV en paralelo y escribe un CSV.
# uso: run_bpp.sh <salida.csv> <n instancias por (clase,n)> <procesos> -- <opciones de BSG_BPP>
out=$1; ninst=$2; procs=$3; shift 4
dir=$(dirname "$0")/../benchs/MPV/instances
bin=$(dirname "$0")/../../../build/BSG_BPP
echo "instance,class,n,bins,lb,construction,swo,time" > "$out"
for c in 1 4 5 6 7 8; do for n in 50 100 150 200; do for i in $(seq 1 $ninst); do echo "c${c}_${n}_${i}"; done; done; done |
xargs -P "$procs" -I{} sh -c "$bin $dir/{}.txt $* | head -1 | awk -v f={} '{split(f,a,\"_\"); sub(\"c\",\"\",a[1]); print f\",\"a[1]\",\"a[2]\",\"\$2\",\"\$4\",\"\$6\",\"\$8\",\"\$12}'" >> "$out"
