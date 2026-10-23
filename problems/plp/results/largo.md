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
