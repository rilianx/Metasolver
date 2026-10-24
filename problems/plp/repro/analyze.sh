#!/bin/bash
# Certifica (en aritmetica racional exacta) las soluciones de la tabla principal y genera repro/out/results.md.
# uso: JOBS=14 problems/plp/repro/analyze.sh      (con --paper: copia las trayectorias de Merged al paper y
#      regenera sus figuras)
set -e
JOBS=${JOBS:-14}
HERE=$(cd "$(dirname "$0")" && pwd)
PLP=$(cd "$HERE/.." && pwd)
OUT=${OUT:-$HERE/out/runs}
PY=$HERE/deps/venv/bin/python
cd "$PLP/benchs"
ls "$OUT"/full.s*.Stoyan*.sol "$OUT"/full.s*.Egeblad*.sol "$OUT"/full.s*.Liu*.sol 2>/dev/null | while read s; do
  [ -s "${s%.sol}.cert" ] || echo "$s"; done > "$OUT/../cert_jobs.txt"
echo "certificando $(wc -l < "$OUT/../cert_jobs.txt") soluciones"
cert(){ s=$1; b=$(basename "$s" .sol); inst=$(echo "$b" | sed 's/^[a-z_]*\.s[0-9]*\.//; s/__/\//g; s/_/ /g').json
        "$PY" "$PLP/tools/certify_solution.py" "$inst" "$s" . > "${s%.sol}.cert" 2>&1 || true; }
export -f cert; export PY PLP
xargs -P "$JOBS" -d '\n' -I{} bash -c 'cert "$1"' _ {} < "$OUT/../cert_jobs.txt"
"$PY" "$HERE/analyze.py" "$OUT" > "$OUT/../results.md"
echo "tablas en $(cd "$OUT/.." && pwd)/results.md"
if [ "$1" = --paper ]; then
  cp "$OUT/../merged_trajectories.csv" "$PLP/paper/data/merged_trajectories.csv"
  (cd "$PLP/paper" && "$PY" make_figures.py) && echo "figuras regeneradas en $PLP/paper/figures"
fi
