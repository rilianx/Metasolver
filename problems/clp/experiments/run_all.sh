#!/bin/bash
# Campana completa de experimentos de BSG_ULD (instancias finales de Paquay et al. 2018):
#   1. corridas finales: 3 escenarios x t in {0.1, 1} s x 5 repeticiones
#   2. ablacion (t = 0.1 s)
#   3. barridos de cog_penalty y max_bl (t = 0.1 s)
#   4. curvas del volumen segun el tiempo (tres escenarios) y, en paquay, con las fases de
#      mejora a t
# y un resumen en <salida>/summary.txt.
#
# uso: problems/clp/experiments/run_all.sh <salida> [JOBS]
#   SETTINGS: escenarios a correr (por defecto "paquay adapted one"); la curva de tiempo solo
#   se corre si incluye paquay
#   JOBS: procesos en paralelo (por defecto: nucleos fisicos - 2). Como el limite de tiempo es
#   por reloj de CPU de cada proceso, conviene no usar mas procesos que nucleos fisicos.
# Requiere: cmake, g++, python3 (scipy opcional, solo para compare_hessler.py).
set -e
ROOT=$(cd "$(dirname "$0")/../../.." && pwd)
OUT=$(mkdir -p "$1" && cd "$1" && pwd)
CORES=$(lscpu -p=core,socket 2>/dev/null | grep -v '^#' | sort -u | wc -l)
export JOBS=${2:-$(( CORES>2 ? CORES-2 : 1 ))}
R=$ROOT/problems/clp/experiments/run_uld.sh
S=$ROOT/problems/clp/experiments/summarize.py

# compilacion y datos del entorno
mkdir -p $ROOT/build && (cd $ROOT/build && cmake .. -DCMAKE_BUILD_TYPE=Release > /dev/null && make BSG_ULD > /dev/null)
export BIN=$OUT/BSG_ULD
cp $ROOT/build/BSG_ULD $BIN
{
	echo "fecha: $(date -u)"
	echo "commit: $(cd $ROOT && git rev-parse HEAD)"
	echo "procesos en paralelo: $JOBS"
	lscpu
	free -g
	g++ --version | head -1
} > $OUT/environment.txt

#version del codigo (huella del binario): un directorio terminado con otra version se vuelve a correr
COMMIT=$(md5sum $BIN | cut -c1-32)
run(){ # run <nombre> <escenario> <opciones...>
	local name=$1 conf=$2; shift 2
	[ -f $OUT/$name/.done ] && [ "$(cat $OUT/$name/.done)" = "$COMMIT" ] && return
	rm -rf $OUT/$name
	$R $conf $OUT/$name "$@"
	echo "$COMMIT" > $OUT/$name/.done
	echo "$(date -u +%H:%M) $name"
}

SETTINGS=${SETTINGS:-paquay adapted one}
for r in 1 2 3 4 5; do for t in 0.1 1; do for v in $SETTINGS; do
	run final_${v}_t${t}_r$r $v -t $t
done; done; done

for v in $SETTINGS; do
	for cfg in "coggreedy:--cog_greedy 0.5" "nocogpen:--cog_penalty 0" "notranslate:--no_translate" \
			"corner:--corner_anchor" "nosub:--no_substructure" \
			"reduce1:--reduce_new 1" "elimonly:--eliminate_only" "noreduce:--no_eliminate" \
			"hessler:--theta -1" "simpleblocks:--max_bl 1" "norestart:--restarts 0" "nomust:--must_w 0" "scratch:--scratch" "priority:--priority" "noinsert:--no_ruin" \
			"base:--theta -1 --restarts 0 --corner_anchor --no_regroup --no_eliminate --no_substructure"; do
		n=${cfg%%:*}; a=${cfg#*:}
		[ $n = nosub ] && [ $v = paquay ] && continue
		[ $n = norestart ] && [ $v != one ] && continue
		run abl_${v}_$n $v -t 0.1 $a
	done
	for lam in 0 0.5 1 2 5 30; do run lambda${lam}_$v $v -t 0.1 --cog_penalty $lam; done
	for nb in 250 1000 2000; do run maxbl${nb}_$v $v -t 0.1 --max_bl $nb; done
done

for v in $SETTINGS; do for t in 0.02 0.05 0.2 0.5 2; do run curve_${v}_t$t $v -t $t; done; done
case " $SETTINGS " in *" paquay "*)
	#fases de mejora con el mismo tiempo que la construccion
	for t in 0.02 0.05 0.1 0.2 0.5 1; do run curvefull_t$t paquay -t $t --improve_time 1; done;;
esac

# resumen
{
	for v in paquay:8 adapted:10 one:12; do
		n=${v%%:*}; tb=${v#*:}
		case " $SETTINGS " in *" $n "*) ;; *) continue;; esac
		for t in 0.1 1; do
			echo "== final $n t=$t"
			python3 $S --table $tb $OUT/final_${n}_t${t}_r*
		done
		echo "== ablacion $n (t=0.1, sin...)"
		for d in $OUT/abl_${n}_*; do echo "-- $(basename $d)"; python3 $S --table $tb $d | sed -n 2,3p; done
		echo "== curva de tiempo $n"
		for d in $OUT/curve_${n}_t*; do echo "-- $(basename $d)"; python3 $S --table $tb $d | sed -n 2,3p; done
		echo "== max_bl $n (t=0.1)"
		for d in $OUT/maxbl*_$n; do echo "-- $(basename $d)"; python3 $S --table $tb $d | sed -n 2,3p; done
		echo "== cog_penalty $n (t=0.1)"
		for d in $OUT/lambda*_$n; do echo "-- $(basename $d)"; python3 $S --table $tb $d | sed -n 2,3p; done
	done
	case " $SETTINGS " in *" paquay "*)
		echo "== curva de tiempo (paquay)"
		for d in $OUT/curvefull_t* ; do echo "-- $(basename $d)"; python3 $S --table 8 $d | sed -n 2,3p; done;;
	esac
} > $OUT/summary.txt
echo "listo: $OUT/summary.txt"
