# BSG_PLP: resultados con compactación por LP y bajada de altura con contenedor fijo

Parámetros por defecto (`--strip_time 0.3 --slice 0.1 --step 0.03 --band 0.2`), compilado con ibex/SoPlex.
30 s por instancia (Stoyan 2004/2005) y 10 s (Tollenaere 2025), un hilo, 4 procesos en paralelo.
Respecto a `bsg_plp_v1.md`: 32 instancias mejoran, 10 empeoran y 32 quedan iguales. Todas las soluciones fueron verificadas.

| Instancia | Ítems | H_lb | H (BSG) | H (mejora) | Densidad | Factible |
|---|---|---|---|---|---|---|
| Stoyan et al. 2004/Example1 | 80 | 24.46 | 74.57 | 60.56 | 0.40 | yes |
| Stoyan et al. 2004/Example2 | 20 | 29.00 | 40.73 | 36.00 | 0.21 | yes |
| Stoyan et al. 2004/Example3 | 30 | 29.00 | 61.39 | 58.38 | 0.20 | yes |
| Stoyan et al. 2004/Example4 | 40 | 29.00 | 80.95 | 73.47 | 0.21 | yes |
| Stoyan et al. 2004/Example5 | 50 | 29.00 | 95.85 | 90.68 | 0.21 | yes |
| Stoyan et al. 2005/Example1 | 7 | 16.00 | 17.97 | 17.84 | 0.44 | yes |
| Stoyan et al. 2005/Example2 | 12 | 16.00 | 20.68 | 20.30 | 0.40 | yes |
| Stoyan et al. 2005/Example3 | 25 | 16.00 | 35.60 | 34.15 | 0.39 | yes |
| T2025/DIAMONDS/02 ITEMS | 2 | 6.19 | 9.56 | 9.56 | 0.24 | yes |
| T2025/DIAMONDS/03 ITEMS | 3 | 6.19 | 13.15 | 13.15 | 0.27 | yes |
| T2025/DIAMONDS/04 ITEMS | 4 | 6.19 | 17.85 | 17.85 | 0.31 | yes |
| T2025/DIAMONDS/05 ITEMS | 5 | 6.19 | 11.60 | 11.30 | 0.34 | yes |
| T2025/DIAMONDS/06 ITEMS | 6 | 6.19 | 13.78 | 12.73 | 0.35 | yes |
| T2025/DIAMONDS/07 ITEMS | 7 | 6.19 | 16.62 | 14.19 | 0.37 | yes |
| T2025/DIAMONDS/08 ITEMS | 8 | 6.46 | 17.94 | 15.28 | 0.39 | yes |
| T2025/DIAMONDS/09 ITEMS | 9 | 6.70 | 20.13 | 18.10 | 0.37 | yes |
| T2025/DIAMONDS/10 ITEMS | 10 | 7.19 | 21.30 | 18.32 | 0.39 | yes |
| T2025/STOYAN 2004/02 ITEMS | 2 | 12.00 | 16.00 | 16.00 | 0.34 | yes |
| T2025/STOYAN 2004/03 ITEMS | 3 | 13.00 | 20.22 | 20.22 | 0.29 | yes |
| T2025/STOYAN 2004/04 ITEMS | 4 | 16.00 | 27.70 | 27.70 | 0.40 | yes |
| T2025/STOYAN 2004/05 ITEMS | 5 | 21.00 | 28.00 | 28.00 | 0.35 | yes |
| T2025/STOYAN 2004/06 ITEMS | 6 | 21.00 | 28.44 | 28.37 | 0.37 | yes |
| T2025/STOYAN 2004/07 ITEMS | 7 | 21.00 | 31.61 | 31.61 | 0.38 | yes |
| T2025/STOYAN 2004/08 ITEMS | 8 | 21.00 | 36.21 | 36.21 | 0.37 | yes |
| T2025/STOYAN 2004/09 ITEMS | 9 | 21.00 | 42.00 | 42.00 | 0.40 | yes |
| T2025/STOYAN 2004/10 ITEMS | 10 | 21.00 | 31.00 | 31.00 | 0.41 | yes |
| T2025/STOYAN 2004/11 ITEMS | 11 | 21.00 | 31.79 | 31.79 | 0.42 | yes |
| T2025/STOYAN 2004/12 ITEMS | 12 | 21.00 | 33.37 | 33.09 | 0.43 | yes |
| T2025/STOYAN 2004/13 ITEMS | 13 | 21.00 | 37.00 | 37.00 | 0.41 | yes |
| T2025/STOYAN 2004/14 ITEMS | 14 | 21.00 | 40.92 | 40.63 | 0.43 | yes |
| T2025/STOYAN 2004/15 ITEMS | 15 | 21.00 | 34.72 | 34.00 | 0.40 | yes |
| T2025/STOYAN 2004/16 ITEMS | 16 | 21.00 | 37.70 | 35.94 | 0.39 | yes |
| T2025/STOYAN 2004/17 ITEMS | 17 | 21.00 | 38.94 | 38.00 | 0.38 | yes |
| T2025/STOYAN 2004/18 ITEMS | 18 | 21.00 | 40.00 | 38.00 | 0.40 | yes |
| T2025/STOYAN 2004/19 ITEMS | 19 | 21.00 | 42.67 | 41.26 | 0.40 | yes |
| T2025/STOYAN 2004/20 ITEMS | 20 | 21.00 | 46.80 | 44.93 | 0.40 | yes |
| T2025/STOYAN 2005/02 ITEMS | 2 | 10.00 | 12.00 | 12.00 | 0.29 | yes |
| T2025/STOYAN 2005/03 ITEMS | 3 | 10.00 | 17.00 | 17.00 | 0.32 | yes |
| T2025/STOYAN 2005/04 ITEMS | 4 | 10.00 | 17.00 | 17.00 | 0.42 | yes |
| T2025/STOYAN 2005/05 ITEMS | 5 | 12.00 | 15.00 | 15.00 | 0.42 | yes |
| T2025/STOYAN 2005/06 ITEMS | 6 | 16.00 | 16.00 | 16.00 | 0.46 | yes |
| T2025/STOYAN 2005/07 ITEMS | 7 | 16.00 | 17.97 | 17.84 | 0.44 | yes |
| T2025/STOYAN 2005/08 ITEMS | 8 | 16.00 | 23.86 | 23.00 | 0.41 | yes |
| T2025/STOYAN 2005/09 ITEMS | 9 | 16.00 | 25.41 | 25.23 | 0.42 | yes |
| T2025/STOYAN 2005/10 ITEMS | 10 | 16.00 | 28.44 | 27.00 | 0.43 | yes |
| T2025/STOYAN 2005/11 ITEMS | 11 | 16.00 | 20.00 | 20.00 | 0.38 | yes |
| T2025/STOYAN 2005/12 ITEMS | 12 | 16.00 | 21.27 | 21.25 | 0.38 | yes |
| T2025/STOYAN 2005/13 ITEMS | 13 | 16.00 | 22.14 | 21.28 | 0.41 | yes |
| T2025/STOYAN 2005/14 ITEMS | 14 | 16.00 | 22.54 | 21.47 | 0.43 | yes |
| T2025/STOYAN 2005/15 ITEMS | 15 | 16.00 | 29.06 | 26.00 | 0.37 | yes |
| T2025/STOYAN 2005/16 ITEMS | 16 | 16.00 | 27.69 | 27.41 | 0.37 | yes |
| T2025/STOYAN 2005/17 ITEMS | 17 | 16.00 | 30.67 | 27.31 | 0.41 | yes |
| T2025/STOYAN 2005/18 ITEMS | 18 | 16.00 | 31.23 | 28.97 | 0.39 | yes |
| T2025/STOYAN 2005/19 ITEMS | 19 | 16.00 | 35.38 | 32.22 | 0.37 | yes |
| T2025/STOYAN 2005/20 ITEMS | 20 | 16.00 | 35.98 | 32.82 | 0.38 | yes |
| T2025/TETRAHEDRONS/02 ITEMS | 2 | 10.00 | 12.01 | 12.01 | 0.11 | yes |
| T2025/TETRAHEDRONS/03 ITEMS | 3 | 10.00 | 17.01 | 17.01 | 0.11 | yes |
| T2025/TETRAHEDRONS/04 ITEMS | 4 | 10.00 | 19.51 | 19.51 | 0.13 | yes |
| T2025/TETRAHEDRONS/05 ITEMS | 5 | 10.00 | 24.51 | 24.51 | 0.13 | yes |
| T2025/TETRAHEDRONS/06 ITEMS | 6 | 10.00 | 16.91 | 16.91 | 0.13 | yes |
| T2025/TETRAHEDRONS/07 ITEMS | 7 | 10.00 | 18.45 | 18.45 | 0.14 | yes |
| T2025/TETRAHEDRONS/08 ITEMS | 8 | 10.00 | 20.00 | 20.00 | 0.14 | yes |
| T2025/TETRAHEDRONS/09 ITEMS | 9 | 10.00 | 20.36 | 20.36 | 0.16 | yes |
| T2025/TETRAHEDRONS/10 ITEMS | 10 | 10.00 | 22.68 | 22.68 | 0.16 | yes |
| T2025/TETRAHEDRONS/11 ITEMS | 11 | 10.00 | 25.33 | 25.12 | 0.16 | yes |
| T2025/TETRAHEDRONS/12 ITEMS | 12 | 10.00 | 27.86 | 27.86 | 0.16 | yes |
| T2025/TETRAHEDRONS/13 ITEMS | 13 | 10.00 | 29.59 | 29.59 | 0.16 | yes |
| T2025/TETRAHEDRONS/14 ITEMS | 14 | 10.00 | 30.36 | 30.36 | 0.17 | yes |
| T2025/TETRAHEDRONS/15 ITEMS | 15 | 10.00 | 32.86 | 32.09 | 0.17 | yes |
| T2025/TETRAHEDRONS/16 ITEMS | 16 | 10.00 | 34.59 | 34.59 | 0.17 | yes |
| T2025/TETRAHEDRONS/17 ITEMS | 17 | 10.00 | 37.79 | 37.68 | 0.16 | yes |
| T2025/TETRAHEDRONS/18 ITEMS | 18 | 10.00 | 38.81 | 38.22 | 0.17 | yes |
| T2025/TETRAHEDRONS/19 ITEMS | 19 | 10.00 | 40.36 | 40.36 | 0.17 | yes |
| T2025/TETRAHEDRONS/20 ITEMS | 20 | 10.00 | 42.86 | 42.21 | 0.17 | yes |
