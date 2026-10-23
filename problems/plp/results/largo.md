# Instancias difíciles con más tiempo (120 s)

Commit `ddb3cac` (oscilación y kicks) para Stoyan 2004; Stoyan 2005 repetido tras corregir `polytope7` (sus 10 vértices).
3 semillas; media (mejor). `BSG x%`: proporción del tiempo para la búsqueda inicial con BSG (`--strip_time`).
Todas las soluciones verificadas; las de Stoyan 2005 además con `tools/check_convex.py`.

| Instancia | Mejor conocida | 30 s | 120 s, BSG 5% | 120 s, BSG 30% | 120 s, BSG 60% |
|---|---|---|---|---|---|
| Stoyan 2004 Example1 | — | 55.06 (54.41) | 53.48 (53.30) | 54.02 (53.16) | 54.06 (53.23) |
| Stoyan 2004 Example2 | 28.47 | 31.89 (31.13) | 30.98 (30.00) | 31.85 (31.50) | 31.47 (30.38) |
| Stoyan 2004 Example3 | 41.30 | 47.55 (46.34) | 45.91 (45.30) | 47.48 (46.43) | 46.46 (44.35) |
| Stoyan 2004 Example4 | 54.83 | 63.79 (62.00) | 61.10 (60.01) | 60.17 (59.32) | 60.60 (59.21) |
| Stoyan 2004 Example5 | 72.33 | 75.99 (74.81) | 75.43 (75.00) | 74.83 (72.73) | 74.78 (72.73) |
| Stoyan 2005 Example2 | 19.01 | 20.11 (20.00) | — | — | 20.57 (20.46) |
| Stoyan 2005 Example3 | 29.76 | 30.63 (29.41) | — | — | 28.98 (28.01) |

Mejor conocida: Lamas-Fernández et al. (2023) para Stoyan 2004 Ex2–5 y Stoyan 2005 Ex3 (piezas voxelizadas, algo
más grandes; mejor de 10 corridas de 1 h); Egeblad et al. (2009) para Stoyan 2005 Ex2 (19.01) y Ex3 (29.82, 3DNEST).

Stoyan 2005 Ex3: mejor solución 28.01 (`solutions/Stoyan2005_Example3_28.01.json`), bajo 29.76 y 29.82.

El tiempo del BSG inicial influye poco en el resultado final (brecha media en Stoyan 2004 Ex2–5: +7.2%, +7.6% y +6.9%
con 5%, 30% y 60%); con 30 s la reparación con BSG no se ejecuta (la oscilación usa todo el tiempo).

## Kicks de varias piezas (`--kick_size`)

120 s, 3 semillas, media (mejor). Las 63 soluciones fueron verificadas con `tools/check_solution.py` (incluidas las
no convexas de Stoyan 2004).

| Instancia | Mejor conocida | k = 1 | k ≤ 2 | k ≤ 3 |
|---|---|---|---|---|
| Stoyan 2004 Example1 | — | 53.77 (53.09) | 54.02 (53.75) | 54.12 (53.40) |
| Stoyan 2004 Example2 | 28.47 | 31.81 (31.40) | 31.05 (29.60) | 32.01 (32.00) |
| Stoyan 2004 Example3 | 41.30 | 47.53 (46.53) | 47.02 (45.00) | 48.10 (48.00) |
| Stoyan 2004 Example4 | 54.83 | 60.42 (59.28) | 60.93 (59.28) | 61.03 (59.46) |
| Stoyan 2004 Example5 | 72.33 | 74.53 (73.73) | 75.83 (74.31) | 75.54 (74.00) |
| Stoyan 2005 Example2 | 19.01 | 20.82 (20.74) | 20.55 (20.33) | 20.58 (20.29) |
| Stoyan 2005 Example3 | 29.76 | 29.00 (28.63) | 29.80 (29.00) | 29.78 (29.27) |
| Brecha media | | +7.8% | +7.9% | +8.8% |

Reinsertar varias piezas no mejora; `--kick_size` queda en 1.

## MILP sobre una ventana de piezas (`tools/milp_window.py`, HiGHS 1.15)

Stoyan 2005 Example2, desde la mejor solución (20.29); se optimizan las k piezas más altas con las demás fijas:

| k | binarias | resultado | tiempo |
|---|---|---|---|
| 3 | 1232 | óptimo de la ventana = 20.29 (sin mejora) | < 1 min |
| 5 | 1946 | óptimo de la ventana = 20.29 (sin mejora) | < 2 min |
| 8 | 2640 | óptimo de la ventana = 20.29 (sin mejora) | 178 s |
| 12 (modelo completo) | 3036 | sin solución factible en 300 s (cota 16) | 300 s |

La solución es localmente óptima para ventanas de hasta 8 de las 12 piezas: la mejor conocida (19.01) requiere
cambiar también las piezas de abajo.

## Movimientos de mínimo solapamiento (`--sep_method`)

120 s, 3 semillas, media (mejor). `lp`: solo el LP de separación (versión anterior); `axis`: solo los movimientos de
mínimo solapamiento por los ejes con búsqueda local guiada; `both`: ambos (por defecto).

verificadas 63 (check_solution.py), infactibles 0

| Instancia | Mejor conocida | solo LP | solo ejes | ambos |
|---|---|---|---|---|
| Stoyan 2004 Example1 | — | 54.19 (53.68) | 55.37 (54.54) | 52.43 (52.01) |
| Stoyan 2004 Example2 | 28.47 | 31.28 (29.80) | 29.29 (29.00) | 29.88 (29.00) |
| Stoyan 2004 Example3 | 41.30 | 48.02 (48.00) | 42.23 (41.00) | 42.09 (41.84) |
| Stoyan 2004 Example4 | 54.83 | 63.38 (62.22) | 57.99 (56.73) | 56.52 (55.55) |
| Stoyan 2004 Example5 | 72.33 | 74.31 (72.42) | 72.02 (69.98) | 72.68 (71.25) |
| Stoyan 2005 Example2 | 19.01 | 20.49 (20.00) | 19.13 (19.13) | 19.13 (19.13) |
| Stoyan 2005 Example3 | 29.76 | 29.97 (29.18) | 28.49 (28.08) | 28.34 (28.00) |
| Brecha media | | +8.8% | +1.1% | +1.0% |

Mejores soluciones (`solutions/`, verificadas con `tools/check_solution.py` y `PLP_check` con eps = 1e-9):

| Instancia | Mejor publicada | Nuestra mejor |
|---|---|---|
| Stoyan 2004 Example1 | — | 52.01 |
| Stoyan 2004 Example2 | 28.47 | 29.00 |
| Stoyan 2004 Example3 | 41.30 | **41.00** |
| Stoyan 2004 Example4 | 54.83 | 55.55 |
| Stoyan 2004 Example5 | 72.33 | **69.98** |
| Stoyan 2005 Example2 | 19.01 | 19.13 |
| Stoyan 2005 Example3 | 29.76 | **28.00** |

Las mejores publicadas de Stoyan 2004 y de Stoyan 2005 Ex3 son de Lamas-Fernández et al. (2023), con piezas voxelizadas (algo más grandes) y el mejor de 10 corridas de 1 h; la de Stoyan 2005 Ex2 es de Egeblad et al. (2009).
