#!/bin/bash
# Prepara todo lo necesario para reproducir los experimentos del paper:
#   1. ibex (rama dfb-sc-paper, con SoPlex incluido) en repro/deps/ibex-install
#   2. BSG_PLP y PLP_check (con ibex y Qhull) en <repo>/build-repro
#   3. entorno Python en repro/deps/venv (numpy, scipy, trimesh, rtree, coacd, matplotlib)
#   4. instancia Chess (ESICUP) y sus descomposiciones conservadoras (decompositions_t05 y decompositions_C)
#   5. prueba rapida
# uso: JOBS=14 problems/plp/repro/setup.sh
# requiere (Debian/Ubuntu): sudo apt install build-essential cmake git flex bison pkg-config libqhull-dev python3-venv
set -e
JOBS=${JOBS:-14}
HERE=$(cd "$(dirname "$0")" && pwd)
PLP=$(cd "$HERE/.." && pwd)
REPO=$(cd "$PLP/../.." && pwd)
DEPS=$HERE/deps
mkdir -p "$DEPS" "$HERE/out"

echo "== 0. dependencias del sistema"
miss=""
for c in g++ cmake make git flex bison pkg-config python3; do command -v $c >/dev/null || miss="$miss $c"; done
[ -f /usr/include/libqhull_r/libqhull_r.h ] || miss="$miss libqhull-dev"
python3 -c 'import venv, ensurepip' 2>/dev/null || miss="$miss python3-venv"
if [ -n "$miss" ]; then
  echo "faltan:$miss"
  echo "instalar con: sudo apt install build-essential cmake git flex bison pkg-config libqhull-dev python3-venv"
  exit 1
fi
"$HERE/hardware.sh" "$HERE/out/hardware.txt"

echo "== 1. ibex"
IBEX_SRC=$DEPS/ibex-lib; IBEX=$DEPS/ibex-install
IBEX_COMMIT=45877fd4
if [ ! -f "$IBEX/share/pkgconfig/ibex.pc" ] && [ ! -f "$IBEX/lib/pkgconfig/ibex.pc" ]; then
  [ -d "$IBEX_SRC" ] || git clone -b dfb-sc-paper https://github.com/rilianx/ibex-lib.git "$IBEX_SRC"
  git -C "$IBEX_SRC" checkout -q $IBEX_COMMIT
  cmake -S "$IBEX_SRC" -B "$DEPS/ibex-build" -DCMAKE_BUILD_TYPE=Release -DLP_LIB=soplex -DINTERVAL_LIB=gaol \
        -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX="$IBEX" > "$DEPS/ibex-cmake.log"
  cmake --build "$DEPS/ibex-build" -j $JOBS > "$DEPS/ibex-build.log"
  cmake --install "$DEPS/ibex-build" > /dev/null
fi
echo "ibex en $IBEX"

echo "== 2. BSG_PLP"
cmake -S "$REPO" -B "$REPO/build-repro" -DCMAKE_BUILD_TYPE=Release -DIBEX_DIR="$IBEX" | grep -E 'PLP|Qhull|qhull' || true
cmake --build "$REPO/build-repro" -j $JOBS --target BSG_PLP PLP_check > "$DEPS/metasolver-build.log"
grep -q PLP_WITH_IBEX "$REPO/build-repro/CMakeFiles/plp.dir/flags.make" || { echo "BSG_PLP compilado sin ibex"; exit 1; }
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
