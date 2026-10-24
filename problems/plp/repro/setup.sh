#!/bin/bash
# Prepara todo lo necesario para reproducir los experimentos del paper:
#   1. SoPlex (v8.1.0, solo la biblioteca, sin GMP/Boost/zlib) en repro/deps/soplex-install
#   2. BSG_PLP y PLP_check (con SoPlex y Qhull) en <repo>/build-repro
#   3. entorno Python en repro/deps/venv (numpy, scipy, trimesh, rtree, coacd, matplotlib)
#   4. instancia Chess (ESICUP) y sus descomposiciones conservadoras (decompositions_t05 y decompositions_C)
#   5. prueba rapida
# uso: JOBS=14 problems/plp/repro/setup.sh
# requiere (Debian/Ubuntu): sudo apt install build-essential cmake git libqhull-dev python3-venv
set -e
JOBS=${JOBS:-14}
HERE=$(cd "$(dirname "$0")" && pwd)
PLP=$(cd "$HERE/.." && pwd)
REPO=$(cd "$PLP/../.." && pwd)
DEPS=$HERE/deps
mkdir -p "$DEPS" "$HERE/out"

echo "== 0. dependencias del sistema"
miss=""
for c in g++ cmake make git python3; do command -v $c >/dev/null || miss="$miss $c"; done
[ -f /usr/include/libqhull_r/libqhull_r.h ] || miss="$miss libqhull-dev"
python3 -c 'import venv, ensurepip' 2>/dev/null || miss="$miss python3-venv"
if [ -n "$miss" ]; then
  echo "faltan:$miss"
  echo "instalar con: sudo apt install build-essential cmake git libqhull-dev python3-venv"
  exit 1
fi
"$HERE/hardware.sh" "$HERE/out/hardware.txt"

echo "== 1. SoPlex"
SPX=$DEPS/soplex-install
SOPLEX_TAG=v8.1.0
if [ ! -f "$SPX/lib/cmake/soplex/soplex-config.cmake" ]; then
  [ -d "$DEPS/soplex" ] || git clone -q --depth 1 -b $SOPLEX_TAG https://github.com/scipopt/soplex.git "$DEPS/soplex"
  cmake -S "$DEPS/soplex" -B "$DEPS/soplex-build" -DCMAKE_BUILD_TYPE=Release -DZLIB=off -DGMP=off -DBOOST=off \
        -DMPFR=off -DPAPILO=off -DQUADMATH=off -DCMAKE_INSTALL_PREFIX="$SPX" > "$DEPS/soplex-cmake.log"
  cmake --build "$DEPS/soplex-build" -j $JOBS > "$DEPS/soplex-build.log"
  cmake --install "$DEPS/soplex-build" > /dev/null
fi
echo "SoPlex en $SPX"

echo "== 2. BSG_PLP"
cmake -S "$REPO" -B "$REPO/build-repro" -DCMAKE_BUILD_TYPE=Release -DSOPLEX_DIR="$SPX" | grep -E 'PLP|Qhull|qhull' || true
cmake --build "$REPO/build-repro" -j $JOBS --target BSG_PLP PLP_check > "$DEPS/metasolver-build.log"
grep -q PLP_WITH_SOPLEX "$REPO/build-repro/CMakeFiles/plp.dir/flags.make" || { echo "BSG_PLP compilado sin SoPlex"; exit 1; }
grep -q PLP_WITH_QHULL "$REPO/build-repro/CMakeFiles/plp.dir/flags.make" || { echo "BSG_PLP compilado sin Qhull"; exit 1; }
echo "binarios en $REPO/build-repro"

echo "== 3. Python"
[ -x "$DEPS/venv/bin/python" ] || python3 -m venv "$DEPS/venv"
"$DEPS/venv/bin/pip" install -q --upgrade pip
"$DEPS/venv/bin/pip" install -q numpy scipy trimesh rtree coacd==1.0.14 matplotlib
"$DEPS/venv/bin/pip" freeze | grep -iE 'numpy|scipy|trimesh|rtree|coacd|matplotlib' > "$HERE/out/python-packages.txt"

echo "== 4. Chess (Lamas-Fernandez et al. 2022)"
CH="Lamas-Fernandez, C. et al. 2022"
[ -f "$PLP/benchs/$CH/Chess.json" ] || bash "$PLP/tools/fetch_lamas.sh"
PIECES=()
for p in bishop king knight pawn queen rook; do PIECES+=("$CH/Chess/classic_$p.stl"); done
dec(){ #carpeta y opciones de decompose_mesh.py (se omite si ya existe)
  local d=$1; shift
  if [ ! -f "$PLP/benchs/$d/$CH/Chess/classic_rook.obj" ]; then
    "$DEPS/venv/bin/python" "$PLP/tools/decompose_mesh.py" "$PLP/benchs" --dec $d "$@" "${PIECES[@]}"
  fi
}
dec decompositions_t05 --threshold 0.05 --kdop 0 --merge 0   # fina: 6-20 partes por pieza
dec decompositions_C --threshold 0.2 --kdop 26 --hullfaces 16 --merge 0   # gruesa: 1-4 partes

echo "== 5. prueba rapida"
cd "$PLP/benchs"
"$REPO/build-repro/BSG_PLP" "Stoyan et al. 2005/Example1.json" -t 3 --seed 1 -o "$HERE/out/smoke.sol" | grep -E '^(feasible|height):'
"$REPO/build-repro/PLP_check" "Stoyan et al. 2005/Example1.json" -s "$HERE/out/smoke.sol" -q
"$DEPS/venv/bin/python" "$PLP/tools/certify_solution.py" "Stoyan et al. 2005/Example1.json" "$HERE/out/smoke.sol" . | tail -1
echo "listo. siguiente paso: JOBS=$JOBS $HERE/run_experiments.sh"
