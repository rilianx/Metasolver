# Paper (borrador)

`main.tex` (elsarticle), `refs.bib`. Compilar: `pdflatex main && bibtex main && pdflatex main && pdflatex main`.

Los números en azul (`\prov{}`) son provisionales: se reemplazan con la corrida final (10 semillas, mismo tiempo,
versión final) y el certificado exacto. Las figuras se generan con `python make_figures.py` (numpy, scipy,
matplotlib) a partir de `data/` y `../results/solutions/`.
