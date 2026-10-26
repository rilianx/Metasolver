# Paper

`main.tex` (elsarticle), `refs.bib`. Compilar: `pdflatex main && bibtex main && pdflatex main && pdflatex main`.

Figuras (numpy, scipy, matplotlib):
* `python make_figures.py`: `figures/merged_time.pdf` (desde `data/merged_trajectories.csv`) y `figures/solutions.pdf`
  (desde `../results/solutions_final/`).
* `python make_diagrams.py`: `figures/nfp.pdf`, `figures/axis.pdf`, `figures/construction.pdf` y `figures/overlap.pdf`, diagramas 2D calculados con una
  version 2D del metodo (NFP, penetracion, movimiento exacto por eje, descenso por LP, GLS y compactacion por LP).

Los resultados de las tablas vienen de `../results/servidor/` (ver `../repro/`).
