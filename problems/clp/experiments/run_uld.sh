#!/bin/bash
# Ejecuta BSG_ULD en las 300 instancias finales de Paquay et al. (2018), guarda las soluciones
# y las valida con validate_uld.py.
#
# uso: run_uld.sh <configuracion> <salida> [opciones de BSG_ULD...]
#   configuracion: paquay   (instancias de Paquay et al., comparar con --table 8)
#                  adapted  (borde, soporte por area, CoG relativo; --table 10)
#                  one      (adapted con un ULD de cada tipo; --table 12)
# Variables: BIN (binario, por defecto build/BSG_ULD), JOBS (procesos en paralelo, 4)
#
# ejemplo: problems/clp/experiments/run_uld.sh paquay /tmp/pq -t 0.1
#          python3 problems/clp/compare_hessler.py --table 8 /tmp/pq
set -e
ROOT=$(cd "$(dirname "$0")/../../.." && pwd)
CONF=$1; OUT=$2; shift 2
BIN=${BIN:-$ROOT/build/BSG_ULD}
JOBS=${JOBS:-4}
ADAPTED="--edge 10 --edge_offset 10 --min_overlap 0.9 --padding 10 --cog_rel 0.1"
case $CONF in
	paquay)  ARGS=""; VARGS="";;
	adapted) ARGS="$ADAPTED"; VARGS="$ADAPTED";;
	one)     ARGS="$ADAPTED --uld_count 1"; VARGS="$ADAPTED --allow_unloaded";;
	*) echo "configuracion desconocida: $CONF"; exit 1;;
esac
mkdir -p $OUT
export ROOT BIN OUT ARGS VARGS EXTRA="$*"
ls $ROOT/problems/clp/benchs/ULD/Final_instances/*.csv | xargs -P$JOBS -I{} sh -c '
	f={}; b=$(basename $f .csv)
	$BIN $f $ARGS $EXTRA -v -o $OUT/$b.sol > $OUT/$b.out 2>&1
	python3 $ROOT/problems/clp/validate_uld.py $f $ROOT/problems/clp/benchs/ULD/uld.csv $OUT/$b.sol $VARGS > $OUT/$b.val 2>&1 || true'
