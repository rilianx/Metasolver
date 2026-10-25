# Resultados finales (paper)

> **Resultados definitivos del paper:** `servidor/results.md` (corridas en el servidor, 2× Xeon E5-2620 v4, SoPlex 8.1, 14 procesos, commit 5e61fde) y `solutions_final/`. Lo que sigue corresponde a la versión anterior (ibex, 4 procesos).

Versión por defecto (kick mixto), 120 s, semillas 1–10, 4 corridas en paralelo (Xeon 2.8 GHz).
Brechas respecto de la mejor altura publicada (tabla del paper). 'reinsert': kick solo por reinserción.

```
## Tabla principal (full, 120 s)
Stoyan_et_al._2005__Example1     n=10 media 17.47 mejor 17.46  sd 0.00  infactibles 0
Stoyan_et_al._2005__Example2     n=10 media 19.13 mejor 19.13  sd 0.00  infactibles 0
Stoyan_et_al._2005__Example3     n=10 media 27.46 mejor 26.45  sd 0.38  infactibles 0
Stoyan_et_al._2004__Example1     n=10 media 53.22 mejor 52.31  sd 0.56  infactibles 0
Stoyan_et_al._2004__Example2     n=10 media 29.00 mejor 29.00  sd 0.00  infactibles 0
Stoyan_et_al._2004__Example3     n=10 media 40.38 mejor 40.00  sd 0.32  infactibles 0
Stoyan_et_al._2004__Example4     n=10 media 52.83 mejor 50.64  sd 0.87  infactibles 0
Stoyan_et_al._2004__Example5     n=10 media 66.73 mejor 64.85  sd 1.13  infactibles 0
Egeblad_et_al._2009__Merged1     n=10 media 16.69 mejor 16.58  sd 0.09  infactibles 0
Egeblad_et_al._2009__Merged2     n=10 media 21.01 mejor 21.00  sd 0.02  infactibles 0
Egeblad_et_al._2009__Merged3     n=10 media 22.62 mejor 22.04  sd 0.40  infactibles 0
Egeblad_et_al._2009__Merged4     n=10 media 25.06 mejor 24.00  sd 0.59  infactibles 0
Egeblad_et_al._2009__Merged5     n=10 media 27.89 mejor 27.20  sd 0.25  infactibles 0

## Ablacion (brecha media)
full         S2004 -2.9%  S2005 -2.1%  S2004Ex1 53.22  (n=10)
nolp         S2004 -3.0%  S2005 -2.0%  S2004Ex1 53.02  (n=10)
add          S2004 -1.7%  S2005 -1.5%  S2004Ex1 53.12  (n=10)
reinsert     S2004 -3.1%  S2005 -2.0%  S2004Ex1 52.95  (n=10)
slow (24 s)  S2004 -0.2%  S2005 -1.1%  S2004Ex1 54.33  (n=10)
```

## Tollenaere et al. (2026), 66 instancias, 10 semillas, 30 s

| Serie | Instancias | Óptimo probado (T2025) | Brecha media (media) | Brecha media (mejor) | Mejor igual (±0.5%) | Mejor estrictamente menor que la cota superior |
|---|---|---|---|---|---|---|
| DIAMONDS | 9 | 4 | -2.35% | -2.85% | 5 | 5 |
| STOYAN 2004 | 19 | 9 | -2.63% | -3.06% | 10 | 9 |
| STOYAN 2005 | 19 | 9 | -2.52% | -3.01% | 9 | 9 |
| TETRAHEDRONS | 19 | 5 | -1.26% | -1.30% | 13 | 9 |

DIAMONDS       2  ref 9.56  nuestra 9.56 (9.56)  +0.0% (+0.0%)  n=10
DIAMONDS       3  ref 13.15  nuestra 13.15 (13.15)  -0.0% (-0.0%)  n=10
DIAMONDS       4  ref 17.85  nuestra 17.85 (17.85)  -0.0% (-0.0%)  n=10
DIAMONDS       5  ref 10.69  nuestra 10.70 (10.69)  +0.1% (-0.0%)  n=10
DIAMONDS       6  ref 12.14 (UB; LB 8.61)  nuestra 12.12 (12.10)  -0.2% (-0.3%)  n=10
DIAMONDS       7  ref 15.02 (UB; LB 8.32)  nuestra 14.04 (13.98)  -6.5% (-6.9%)  n=10
DIAMONDS       8  ref 16.23 (UB; LB 8.11)  nuestra 15.29 (15.25)  -5.8% (-6.0%)  n=10
DIAMONDS       9  ref 17.47 (UB; LB 7.84)  nuestra 16.95 (16.54)  -3.0% (-5.3%)  n=10
DIAMONDS      10  ref 18.87 (UB; LB 7.85)  nuestra 17.78 (17.55)  -5.8% (-7.0%)  n=10
STOYAN 2004    2  ref 16.00  nuestra 16.00 (16.00)  +0.0% (+0.0%)  n=10
STOYAN 2004    3  ref 19.30  nuestra 19.30 (19.30)  +0.0% (+0.0%)  n=10
STOYAN 2004    4  ref 27.53  nuestra 27.53 (27.53)  +0.0% (+0.0%)  n=10
STOYAN 2004    5  ref 28.00  nuestra 28.00 (28.00)  +0.0% (+0.0%)  n=10
STOYAN 2004    6  ref 28.00  nuestra 28.00 (28.00)  +0.0% (+0.0%)  n=10
STOYAN 2004    7  ref 31.61  nuestra 31.63 (31.61)  +0.0% (+0.0%)  n=10
STOYAN 2004    8  ref 34.59  nuestra 34.59 (34.59)  -0.0% (-0.0%)  n=10
STOYAN 2004    9  ref 40.92  nuestra 40.99 (40.92)  +0.2% (+0.0%)  n=10
STOYAN 2004   10  ref 30.19  nuestra 30.23 (30.19)  +0.1% (+0.0%)  n=10
STOYAN 2004   11  ref 32.31 (UB; LB 28.00)  nuestra 31.18 (31.09)  -3.5% (-3.8%)  n=10
STOYAN 2004   12  ref 32.78 (UB; LB 27.90)  nuestra 32.80 (32.80)  +0.1% (+0.1%)  n=10
STOYAN 2004   13  ref 36.87 (UB; LB 27.47)  nuestra 35.38 (35.37)  -4.0% (-4.1%)  n=10
STOYAN 2004   14  ref 40.05 (UB; LB 24.00)  nuestra 37.95 (37.73)  -5.2% (-5.8%)  n=10
STOYAN 2004   15  ref 32.70 (UB; LB 21.00)  nuestra 31.10 (30.60)  -4.9% (-6.4%)  n=10
STOYAN 2004   16  ref 33.35 (UB; LB 21.00)  nuestra 32.14 (32.00)  -3.6% (-4.0%)  n=10
STOYAN 2004   17  ref 36.75 (UB; LB 21.00)  nuestra 33.34 (32.83)  -9.3% (-10.7%)  n=10
STOYAN 2004   18  ref 37.60 (UB; LB 21.00)  nuestra 35.09 (34.72)  -6.7% (-7.7%)  n=10
STOYAN 2004   19  ref 40.00 (UB; LB 21.00)  nuestra 37.71 (37.10)  -5.7% (-7.3%)  n=10
STOYAN 2004   20  ref 44.00 (UB; LB 21.00)  nuestra 40.70 (40.22)  -7.5% (-8.6%)  n=10
STOYAN 2005    2  ref 12.00  nuestra 12.00 (12.00)  +0.0% (+0.0%)  n=10
STOYAN 2005    3  ref 16.73  nuestra 16.73 (16.73)  -0.0% (-0.0%)  n=10
STOYAN 2005    4  ref 17.00  nuestra 17.00 (17.00)  +0.0% (+0.0%)  n=10
STOYAN 2005    5  ref 15.00  nuestra 15.00 (15.00)  +0.0% (+0.0%)  n=10
STOYAN 2005    6  ref 16.00  nuestra 16.00 (16.00)  +0.0% (+0.0%)  n=10
STOYAN 2005    7  ref 17.46  nuestra 17.47 (17.46)  +0.0% (+0.0%)  n=10
STOYAN 2005    8  ref 22.71  nuestra 22.71 (22.71)  +0.0% (+0.0%)  n=10
STOYAN 2005    9  ref 24.93  nuestra 24.93 (24.93)  -0.0% (-0.0%)  n=10
STOYAN 2005   10  ref 26.05  nuestra 26.20 (26.20)  +0.6% (+0.6%)  n=10
STOYAN 2005   11  ref 18.00 (UB; LB 16.00)  nuestra 18.00 (18.00)  +0.0% (+0.0%)  n=10
STOYAN 2005   12  ref 20.07 (UB; LB 16.00)  nuestra 19.13 (19.13)  -4.7% (-4.7%)  n=10
STOYAN 2005   13  ref 20.76 (UB; LB 16.00)  nuestra 20.35 (20.22)  -2.0% (-2.6%)  n=10
STOYAN 2005   14  ref 21.93 (UB; LB 16.00)  nuestra 21.13 (21.00)  -3.7% (-4.2%)  n=10
STOYAN 2005   15  ref 23.00 (UB; LB 16.00)  nuestra 22.59 (22.36)  -1.8% (-2.8%)  n=10
STOYAN 2005   16  ref 24.90 (UB; LB 16.00)  nuestra 23.04 (23.00)  -7.5% (-7.6%)  n=10
STOYAN 2005   17  ref 26.00 (UB; LB 16.00)  nuestra 23.98 (23.51)  -7.8% (-9.6%)  n=10
STOYAN 2005   18  ref 27.02 (UB; LB 16.00)  nuestra 25.69 (25.00)  -4.9% (-7.5%)  n=10
STOYAN 2005   19  ref 28.38 (UB; LB 16.00)  nuestra 26.20 (25.79)  -7.7% (-9.1%)  n=10
STOYAN 2005   20  ref 29.90 (UB; LB 16.00)  nuestra 27.37 (27.00)  -8.4% (-9.7%)  n=10
TETRAHEDRONS   2  ref 12.01  nuestra 12.01 (12.01)  -0.0% (-0.0%)  n=10
TETRAHEDRONS   3  ref 17.01  nuestra 17.01 (17.01)  -0.0% (-0.0%)  n=10
TETRAHEDRONS   4  ref 19.51  nuestra 19.51 (19.51)  -0.0% (-0.0%)  n=10
TETRAHEDRONS   5  ref 24.51  nuestra 24.51 (24.51)  -0.0% (-0.0%)  n=10
TETRAHEDRONS   6  ref 15.18  nuestra 15.18 (15.18)  -0.0% (-0.0%)  n=10
TETRAHEDRONS   7  ref 18.45 (UB; LB 15.18)  nuestra 18.45 (18.45)  +0.0% (+0.0%)  n=10
TETRAHEDRONS   8  ref 20.00 (UB; LB 10.00)  nuestra 20.00 (20.00)  +0.0% (+0.0%)  n=10
TETRAHEDRONS   9  ref 20.36 (UB; LB 10.00)  nuestra 20.36 (20.36)  -0.0% (-0.0%)  n=10
TETRAHEDRONS  10  ref 22.68 (UB; LB 10.00)  nuestra 22.68 (22.68)  -0.0% (-0.0%)  n=10
TETRAHEDRONS  11  ref 24.86 (UB; LB 10.00)  nuestra 24.75 (24.75)  -0.4% (-0.4%)  n=10
TETRAHEDRONS  12  ref 27.59 (UB; LB 10.00)  nuestra 27.40 (27.40)  -0.7% (-0.7%)  n=10
TETRAHEDRONS  13  ref 29.91 (UB; LB 10.00)  nuestra 29.59 (29.59)  -1.1% (-1.1%)  n=10
TETRAHEDRONS  14  ref 30.36 (UB; LB 10.00)  nuestra 30.36 (30.36)  -0.0% (-0.0%)  n=10
TETRAHEDRONS  15  ref 33.44 (UB; LB 10.00)  nuestra 32.09 (32.09)  -4.0% (-4.0%)  n=10
TETRAHEDRONS  16  ref 34.59 (UB; LB 10.00)  nuestra 34.43 (34.43)  -0.5% (-0.5%)  n=10
TETRAHEDRONS  17  ref 37.52 (UB; LB 10.00)  nuestra 35.84 (35.72)  -4.5% (-4.8%)  n=10
TETRAHEDRONS  18  ref 40.36 (UB; LB 10.00)  nuestra 38.22 (38.22)  -5.3% (-5.3%)  n=10
TETRAHEDRONS  19  ref 40.54 (UB; LB 10.00)  nuestra 40.45 (40.36)  -0.2% (-0.4%)  n=10
TETRAHEDRONS  20  ref 43.81 (UB; LB 10.00)  nuestra 40.68 (40.54)  -7.1% (-7.5%)  n=10

Certificación exacta de las 130 soluciones `full`: 129 certificadas; la restante (Merged5, semilla 4) tiene un par
en contacto con solapamiento 3.7e-10. Mejores soluciones certificadas: `solutions_final/`.

## Liu et al. (2015), Experiment1 (36 piezas; prismas descompuestos de forma exacta)

120 s, semillas 1–10, versión del paper (la misma de las demás tablas): media 29.94, mejor 29.41 (certificada), peor
30.15. Con la versión posterior (Qhull, fusión de caras casi coplanares): media 30.08, mejor 29.58, peor 30.95. Referencias: Lamas-Fernández et al.
(vóxeles, 10 × 1 h): ILS 36.40 / 38.58, TS 34.13 / 35.12, VNS 34.27 / 35.37 (mejor / media); HAPE3D (Liu et al.
2015) 31.2 con 8 rotaciones permitidas.

## Chess (Lamas-Fernández et al.), descomposición conservadora (CoACD + escalado)

| Configuración | Tiempo | Altura (semilla 1; semilla 2) |
|---|---|---|
| umbral 0.05 (+4–12 % de volumen) | 1200 s | 147.24; 149.20 |
| umbral 0.10 (+6–28 %) | 1200 s | 153.71; 152.73 |
| umbral 0.20 + k-DOP (+38–78 %) | 1200 s | 151.31 |
| multirresolución (umbral 0.20 + k-DOP 900 s, luego umbral 0.05 300 s) | 1200 s | 147.85; 152.74 |
| umbral 0.20 + k-DOP + pseudobloques | 300 s | 156.98 (5 bloques), 157.43 (20) |
| Lamas-Fernández et al., VNS (vóxeles de 1 unidad) | 3600 s | 131 (mejor), 135.10 (media) |

Con piezas curvas los NFP entre partes tienen cientos de facetas y cada separación toma 20–35 s (umbral 0.05).
