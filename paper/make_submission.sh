#!/bin/bash
# Arma submission.zip con todas las fuentes del paper en el directorio raiz (sin subdirectorios),
# como lo piden los sistemas de envio de las revistas, y comprueba que compila.
# uso: paper/make_submission.sh
set -e
cd "$(dirname "$0")"
OUT=$(mktemp -d)
cp main.tex introduction.tex problem.tex literature.tex algorithm.tex experiments.tex \
	conclusions.tex references.bib elsarticle.cls elsarticle-harv.bst $OUT/
cp figures/bsg.tex figures/edge.tex figures/loading.pdf figures/liquid.pdf figures/time.pdf $OUT/
# rutas de las figuras en el directorio raiz
sed -i 's#figures/##g' $OUT/*.tex
(cd $OUT && pdflatex -interaction=nonstopmode main >/dev/null && bibtex main >/dev/null \
	&& pdflatex -interaction=nonstopmode main >/dev/null && pdflatex -interaction=nonstopmode main >/dev/null)
grep -q "undefined" $OUT/main.log && { echo "referencias sin definir"; exit 1; }
# se incluye el .bbl por si el sistema no corre bibtex
rm -f submission.zip
(cd $OUT && zip -q ../submission.zip main.tex introduction.tex problem.tex literature.tex \
	algorithm.tex experiments.tex conclusions.tex references.bib main.bbl elsarticle.cls \
	elsarticle-harv.bst bsg.tex edge.tex loading.pdf liquid.pdf time.pdf && mv ../submission.zip "$OLDPWD/")
rm -rf $OUT
unzip -l submission.zip
