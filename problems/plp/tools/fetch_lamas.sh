#!/bin/bash
# Descarga las instancias Chess y Engine de Lamas-Fernandez et al. (repositorio ESICUP) en benchs/.
# Las mallas tienen licencias propias (Chess: GNU GPL; Engine: CC BY-NC); no se incluyen en este repositorio.
# Luego: python tools/decompose_mesh.py benchs "Lamas-Fernandez, C. et al. 2022/Chess/<pieza>.stl" ...
set -e
cd "$(dirname "$0")/../benchs"
tmp=$(mktemp -d)
git clone --depth 1 --filter=blob:none --sparse https://github.com/ESICUP/datasets.git "$tmp/datasets"
git -C "$tmp/datasets" sparse-checkout set "3d_irregular/Lamas-Fernandez, C. et al. 2022"
cp -r "$tmp/datasets/3d_irregular/Lamas-Fernandez, C. et al. 2022" .
rm -rf "$tmp"
