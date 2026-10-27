#!/bin/bash
# Corre todos los experimentos del paper, con JOBS procesos en paralelo (uno por nucleo; un hilo por corrida).
# uso: JOBS=14 problems/plp/repro/run_experiments.sh [grupos...]
#   grupos (por defecto todos): chess main ablation kicks sens tollenaere liu
#   main        13 instancias (Stoyan 2004/2005, Merged1-5) x 10 semillas x 120 s        130 corridas
#   ablation    7 instancias dificiles x 5 variantes (+ LP compaction, + Improve, pesos aditivos, sigma = 0,
#               greedy por volumen) x 10 x 120 s
#   kicks       7 instancias dificiles x 5 kicks (none, mix, reinsert, top, topbest) x 10 x 120 s (por defecto: random)
#   sens        7 instancias dificiles x (rho 0.5 %, 2 %; 100 y 2000 pasadas) x 10 x 120 s
#   tollenaere  66 instancias de Tollenaere et al. 2025 x 10 semillas x 30 s
#   liu         Liu et al. 2015 Experiment1 x 10 semillas x 120 s
#   chess       Chess x 5 semillas: fina 1200 s, gruesa 1200 s, gruesa 900 s + fina 300 s (multirresolucion)
# Tiempo total con 14 procesos: ~3 h. Las corridas ya terminadas (log con "height:") se omiten, asi que el script
# se puede relanzar si se interrumpe. TDIV=n divide todos los tiempos por n (prueba rapida; usar otro OUT).
# Resultados en repro/out/runs (OUT=... para cambiarlo); luego repro/analyze.sh.
set -e
JOBS=${JOBS:-14}
HERE=$(cd "$(dirname "$0")" && pwd)
PLP=$(cd "$HERE/.." && pwd)
REPO=$(cd "$PLP/../.." && pwd)
BIN=${BIN:-$REPO/build-repro/BSG_PLP}
OUT=${OUT:-$HERE/out/runs}
GROUPS_=${*:-chess main ablation kicks sens tollenaere liu}
[ -x "$BIN" ] || { echo "no existe $BIN (correr repro/setup.sh)"; exit 1; }
mkdir -p "$OUT"
phys=$(lscpu -p=Core,Socket 2>/dev/null | grep -v '^#' | sort -u | wc -l)
[ "$phys" -gt 0 ] && [ "$JOBS" -gt "$phys" ] && echo "AVISO: JOBS=$JOBS > nucleos fisicos ($phys); los tiempos no serian comparables"
"$HERE/hardware.sh" "$OUT/../hardware.txt" > /dev/null

# trabajo: config|semilla|tiempo|instancia|opciones   (log y solucion: OUT/config.sSEMILLA.CLAVE.{log,sol})
run(){
  IFS='|' read -r cfg s t inst opts <<< "$1"
  t=$(( (t + TDIV - 1) / TDIV )); t1=$(( (900 + TDIV - 1) / TDIV ))
  key=$(echo "${inst%.json}" | sed 's|/|__|g; s| |_|g')
  base="$OUT/$cfg.s$s.$key"
  grep -q '^height:' "$base.log" 2>/dev/null && return 0
  if [ "$cfg" = chess_mr ]; then   # gruesa y luego fina desde esa solucion
    $BIN "$inst" --dec decompositions_C -t $t1 --seed $s -o "$base.stage1.sol" > "$base.stage1.log" 2>&1
    $BIN "$inst" --dec decompositions_t05 --start "$base.stage1.sol" -t $t --seed $s -o "$base.sol" > "$base.log" 2>&1
  else
    $BIN "$inst" -t $t --seed $s $opts -o "$base.sol" > "$base.log" 2>&1
  fi
}
TDIV=${TDIV:-1}
export -f run; export BIN OUT TDIV

HARD=("Stoyan et al. 2004/Example1.json" "Stoyan et al. 2004/Example2.json" "Stoyan et al. 2004/Example3.json"
      "Stoyan et al. 2004/Example4.json" "Stoyan et al. 2004/Example5.json" "Stoyan et al. 2005/Example2.json"
      "Stoyan et al. 2005/Example3.json")
ALL=("${HARD[@]}" "Stoyan et al. 2005/Example1.json" "Egeblad et al. 2009/Merged1.json" "Egeblad et al. 2009/Merged2.json"
     "Egeblad et al. 2009/Merged3.json" "Egeblad et al. 2009/Merged4.json" "Egeblad et al. 2009/Merged5.json")
CHESS="Lamas-Fernandez, C. et al. 2022/Chess.json"
has(){ [[ " $GROUPS_ " == *" $1 "* ]]; }

cd "$PLP/benchs"
LIST="$OUT/../jobs.txt"
{ # las corridas largas primero (mejor reparto entre procesos)
if has chess; then for s in 1 2 3 4 5; do
  echo "chess_mr|$s|300|$CHESS|"
  echo "chess_fine|$s|1200|$CHESS|--dec decompositions_t05"
  echo "chess_coarse|$s|1200|$CHESS|--dec decompositions_C"
done; fi
if has main; then for s in $(seq 1 10); do for f in "${ALL[@]}"; do echo "full|$s|120|$f|"; done; done; fi
if has liu; then for s in $(seq 1 10); do echo "full|$s|120|Liu et al. 2015/Experiment1.json|"; done; fi
if has ablation; then for s in $(seq 1 10); do for f in "${HARD[@]}"; do
  echo "withlp|$s|120|$f|--lp_compact"; echo "withimprove|$s|120|$f|--improve"; echo "add|$s|120|$f|--gls add"
  echo "slide0|$s|120|$f|--slide 0"; echo "volgreedy|$s|120|$f|--alpha 1000 --beta 1 --gamma 0"
done; done; fi
if has kicks; then for s in $(seq 1 10); do for f in "${HARD[@]}"; do
  for k in none mix reinsert top topbest; do echo "kick_$k|$s|120|$f|--kick $k"; done
done; done; fi
if has sens; then for s in $(seq 1 10); do for f in "${HARD[@]}"; do
  echo "rho05|$s|120|$f|--sep_step 0.005"; echo "rho2|$s|120|$f|--sep_step 0.02"
  echo "pass100|$s|120|$f|--gls_iter 100"; echo "pass2000|$s|120|$f|--gls_iter 2000"
done; done; fi
if has tollenaere; then for s in $(seq 1 10); do for f in "Tollenaere, J. et al. 2025"/Instances/*/*.json; do
  echo "full|$s|30|$f|"; done; done; fi
} > "$LIST"
echo "$(wc -l < "$LIST") corridas ($GROUPS_), $JOBS en paralelo; logs en $OUT"
start=$(date +%s)
xargs -P "$JOBS" -d '\n' -I{} bash -c 'run "$1"' _ {} < "$LIST"
echo "terminado en $(( ($(date +%s)-start)/60 )) min. siguiente paso: $HERE/analyze.sh"
