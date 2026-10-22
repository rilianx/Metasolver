# BSG_PLP: resultados iniciales

Commit `d845337`, parámetros por defecto (`--grid 3 --slide 0.1 --alpha 1 --beta 1 --gamma 1`).
30 s por instancia (Stoyan 2004/2005) y 10 s (Tollenaere 2025), un hilo, 4 procesos en paralelo.
H_lb = max(V/(W·D), altura de la pieza más alta). Todas las soluciones fueron verificadas con `Solution::verify` (eps = 1e-6).

| Instancia | Ítems | H_lb | H (BSG) | H (mejora) | Densidad | Factible |
|---|---|---|---|---|---|---|
| Stoyan et al. 2004/Example1 | 80 | 24.46 | 72.18 | 72.18 | 0.34 | yes |
| Stoyan et al. 2004/Example2 | 20 | 29.00 | 40.00 | 40.00 | 0.19 | yes |
| Stoyan et al. 2004/Example3 | 30 | 29.00 | 59.10 | 59.10 | 0.20 | yes |
| Stoyan et al. 2004/Example4 | 40 | 29.00 | 76.47 | 76.47 | 0.20 | yes |
| Stoyan et al. 2004/Example5 | 50 | 29.00 | 99.83 | 99.81 | 0.19 | yes |
| Stoyan et al. 2005/Example1 | 7 | 16.00 | 17.97 | 17.97 | 0.44 | yes |
| Stoyan et al. 2005/Example2 | 12 | 16.00 | 20.68 | 20.68 | 0.39 | yes |
| Stoyan et al. 2005/Example3 | 25 | 16.00 | 36.00 | 36.00 | 0.37 | yes |
| T2025/DIAMONDS/02 ITEMS | 2 | 6.19 | 9.56 | 9.56 | 0.24 | yes |
| T2025/DIAMONDS/03 ITEMS | 3 | 6.19 | 13.15 | 13.15 | 0.27 | yes |
| T2025/DIAMONDS/04 ITEMS | 4 | 6.19 | 17.85 | 17.85 | 0.31 | yes |
| T2025/DIAMONDS/05 ITEMS | 5 | 6.19 | 11.41 | 11.30 | 0.34 | yes |
| T2025/DIAMONDS/06 ITEMS | 6 | 6.19 | 12.80 | 12.72 | 0.35 | yes |
| T2025/DIAMONDS/07 ITEMS | 7 | 6.19 | 14.71 | 14.71 | 0.36 | yes |
| T2025/DIAMONDS/08 ITEMS | 8 | 6.46 | 15.93 | 15.90 | 0.38 | yes |
| T2025/DIAMONDS/09 ITEMS | 9 | 6.70 | 18.05 | 17.88 | 0.37 | yes |
| T2025/DIAMONDS/10 ITEMS | 10 | 7.19 | 19.57 | 19.57 | 0.37 | yes |
| T2025/STOYAN 2004/02 ITEMS | 2 | 12.00 | 16.00 | 16.00 | 0.34 | yes |
| T2025/STOYAN 2004/03 ITEMS | 3 | 13.00 | 20.22 | 20.22 | 0.29 | yes |
| T2025/STOYAN 2004/04 ITEMS | 4 | 16.00 | 27.70 | 27.70 | 0.40 | yes |
| T2025/STOYAN 2004/05 ITEMS | 5 | 21.00 | 28.00 | 28.00 | 0.35 | yes |
| T2025/STOYAN 2004/06 ITEMS | 6 | 21.00 | 28.44 | 28.44 | 0.37 | yes |
| T2025/STOYAN 2004/07 ITEMS | 7 | 21.00 | 31.61 | 31.61 | 0.38 | yes |
| T2025/STOYAN 2004/08 ITEMS | 8 | 21.00 | 36.21 | 36.21 | 0.37 | yes |
| T2025/STOYAN 2004/09 ITEMS | 9 | 21.00 | 41.00 | 41.00 | 0.41 | yes |
| T2025/STOYAN 2004/10 ITEMS | 10 | 21.00 | 31.00 | 31.00 | 0.41 | yes |
| T2025/STOYAN 2004/11 ITEMS | 11 | 21.00 | 31.79 | 31.79 | 0.42 | yes |
| T2025/STOYAN 2004/12 ITEMS | 12 | 21.00 | 33.37 | 33.37 | 0.43 | yes |
| T2025/STOYAN 2004/13 ITEMS | 13 | 21.00 | 36.81 | 36.81 | 0.41 | yes |
| T2025/STOYAN 2004/14 ITEMS | 14 | 21.00 | 40.93 | 40.93 | 0.42 | yes |
| T2025/STOYAN 2004/15 ITEMS | 15 | 21.00 | 35.11 | 35.11 | 0.38 | yes |
| T2025/STOYAN 2004/16 ITEMS | 16 | 21.00 | 36.01 | 36.01 | 0.38 | yes |
| T2025/STOYAN 2004/17 ITEMS | 17 | 21.00 | 38.06 | 38.06 | 0.38 | yes |
| T2025/STOYAN 2004/18 ITEMS | 18 | 21.00 | 40.00 | 40.00 | 0.38 | yes |
| T2025/STOYAN 2004/19 ITEMS | 19 | 21.00 | 42.25 | 42.19 | 0.40 | yes |
| T2025/STOYAN 2004/20 ITEMS | 20 | 21.00 | 46.06 | 46.06 | 0.39 | yes |
| T2025/STOYAN 2005/02 ITEMS | 2 | 10.00 | 12.00 | 12.00 | 0.29 | yes |
| T2025/STOYAN 2005/03 ITEMS | 3 | 10.00 | 17.00 | 17.00 | 0.32 | yes |
| T2025/STOYAN 2005/04 ITEMS | 4 | 10.00 | 17.00 | 17.00 | 0.42 | yes |
| T2025/STOYAN 2005/05 ITEMS | 5 | 12.00 | 16.00 | 16.00 | 0.39 | yes |
| T2025/STOYAN 2005/06 ITEMS | 6 | 16.00 | 16.00 | 16.00 | 0.46 | yes |
| T2025/STOYAN 2005/07 ITEMS | 7 | 16.00 | 17.97 | 17.97 | 0.44 | yes |
| T2025/STOYAN 2005/08 ITEMS | 8 | 16.00 | 23.86 | 23.86 | 0.39 | yes |
| T2025/STOYAN 2005/09 ITEMS | 9 | 16.00 | 25.41 | 25.41 | 0.42 | yes |
| T2025/STOYAN 2005/10 ITEMS | 10 | 16.00 | 27.41 | 27.40 | 0.43 | yes |
| T2025/STOYAN 2005/11 ITEMS | 11 | 16.00 | 18.32 | 18.32 | 0.41 | yes |
| T2025/STOYAN 2005/12 ITEMS | 12 | 16.00 | 20.68 | 20.68 | 0.39 | yes |
| T2025/STOYAN 2005/13 ITEMS | 13 | 16.00 | 21.94 | 21.94 | 0.40 | yes |
| T2025/STOYAN 2005/14 ITEMS | 14 | 16.00 | 22.54 | 22.30 | 0.42 | yes |
| T2025/STOYAN 2005/15 ITEMS | 15 | 16.00 | 25.00 | 25.00 | 0.39 | yes |
| T2025/STOYAN 2005/16 ITEMS | 16 | 16.00 | 25.88 | 25.88 | 0.40 | yes |
| T2025/STOYAN 2005/17 ITEMS | 17 | 16.00 | 27.00 | 27.00 | 0.41 | yes |
| T2025/STOYAN 2005/18 ITEMS | 18 | 16.00 | 29.81 | 29.81 | 0.38 | yes |
| T2025/STOYAN 2005/19 ITEMS | 19 | 16.00 | 33.68 | 33.68 | 0.36 | yes |
| T2025/STOYAN 2005/20 ITEMS | 20 | 16.00 | 31.25 | 30.90 | 0.41 | yes |
| T2025/TETRAHEDRONS/02 ITEMS | 2 | 10.00 | 12.01 | 12.01 | 0.11 | yes |
| T2025/TETRAHEDRONS/03 ITEMS | 3 | 10.00 | 17.01 | 17.01 | 0.11 | yes |
| T2025/TETRAHEDRONS/04 ITEMS | 4 | 10.00 | 19.51 | 19.51 | 0.13 | yes |
| T2025/TETRAHEDRONS/05 ITEMS | 5 | 10.00 | 24.51 | 24.51 | 0.13 | yes |
| T2025/TETRAHEDRONS/06 ITEMS | 6 | 10.00 | 16.91 | 16.91 | 0.13 | yes |
| T2025/TETRAHEDRONS/07 ITEMS | 7 | 10.00 | 18.45 | 18.45 | 0.14 | yes |
| T2025/TETRAHEDRONS/08 ITEMS | 8 | 10.00 | 20.00 | 20.00 | 0.14 | yes |
| T2025/TETRAHEDRONS/09 ITEMS | 9 | 10.00 | 20.36 | 20.36 | 0.16 | yes |
| T2025/TETRAHEDRONS/10 ITEMS | 10 | 10.00 | 22.68 | 22.68 | 0.16 | yes |
| T2025/TETRAHEDRONS/11 ITEMS | 11 | 10.00 | 25.33 | 25.33 | 0.16 | yes |
| T2025/TETRAHEDRONS/12 ITEMS | 12 | 10.00 | 27.86 | 27.86 | 0.16 | yes |
| T2025/TETRAHEDRONS/13 ITEMS | 13 | 10.00 | 29.59 | 29.59 | 0.16 | yes |
| T2025/TETRAHEDRONS/14 ITEMS | 14 | 10.00 | 30.36 | 30.36 | 0.17 | yes |
| T2025/TETRAHEDRONS/15 ITEMS | 15 | 10.00 | 32.86 | 32.86 | 0.16 | yes |
| T2025/TETRAHEDRONS/16 ITEMS | 16 | 10.00 | 34.59 | 34.59 | 0.17 | yes |
| T2025/TETRAHEDRONS/17 ITEMS | 17 | 10.00 | 37.79 | 37.79 | 0.16 | yes |
| T2025/TETRAHEDRONS/18 ITEMS | 18 | 10.00 | 38.22 | 38.22 | 0.17 | yes |
| T2025/TETRAHEDRONS/19 ITEMS | 19 | 10.00 | 40.36 | 40.36 | 0.17 | yes |
| T2025/TETRAHEDRONS/20 ITEMS | 20 | 10.00 | 42.21 | 42.21 | 0.17 | yes |
