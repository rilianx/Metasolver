#!/bin/bash
# Ejecuta BSG_BPP en las instancias originales de Martello, Pisinger y Vigo (benchs/MPV/original)
# en paralelo, valida cada solucion con validate_bpp.py y escribe un CSV.
# uso: run_mpv.sh <salida.csv> <clases, ej. "1 4 5 6 7 8"> <instancias, ej. "1 2 3"> <procesos> -- <opciones>
# (BSG_BIN: ejecutable; por defecto build/BSG_BPP)
out=$1; classes=$2; insts=$3; procs=$4; shift 5
root=$(cd "$(dirname "$0")/../../.." && pwd)
bin=${BSG_BIN:-$root/build/BSG_BPP}
sol=${out%.csv}_sol; mkdir -p "$sol"
echo "instance,class,n,bins,lb,construction,swo,time,valid" > "$out"
for c in $classes; do for n in 50 100 150 200; do for i in $insts; do echo "$c $n $i"; done; done; done |
xargs -P "$procs" -L 1 sh -c '
f='"$root"'/problems/clp/benchs/MPV/original/class$0/$1.txt
s='"$sol"'/c$0_$1_$2.csv
r=$('"$bin"' $f -i $2 -o $s '"$*"' > $s.out; head -1 $s.out; rm $s.out)
v=$(python3 -I '"$root"'/problems/clp/validate_bpp.py '"$root"'/problems/clp/benchs/MPV/original/class$0/$1.txt $s -i $2 2>&1 | tail -1)
echo "$r" | awk -v c=$0 -v n=$1 -v i=$2 -v v="$v" "{print \"c\"c\"_\"n\"_\"i\",\"c\",\"n\",\"\$2\",\"\$4\",\"\$6\",\"\$8\",\"\$12\",\"v}"
' >> "$out"
