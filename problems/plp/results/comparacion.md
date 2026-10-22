# Comparación con la literatura

Resultados de `BSG_PLP` de `bsg_plp_v2.md` (una corrida por instancia, 30 s en las clásicas y 10 s en las de
Tollenaere 2025, un hilo). Todas las soluciones fueron verificadas con `PLP_check`.

Fuentes:
* **T2025**: Tollenaere, Martínez-Sykora, Wauters (2026). EJOR 331(2):365–380, tablas 11 y B.1–B.4
  (cuatro modelos MILP, 1 h cada uno, Xeon Platinum 8360Y). "Mejor altura" es la menor cota superior de los
  cuatro modelos; "óptimo" indica que algún modelo cerró la brecha.
* **LF2023**: Lamas-Fernández, Bennell, Martínez-Sykora (2023). Operations Research 71(4), tablas 9–11
  (mejor de 10 corridas de 1 h; las piezas están voxelizadas, por lo que son algo más grandes que las originales).
* **E2009**: Egeblad, Nielsen, Brazil (2009), 3DNEST (citado en T2025 y LF2023; mejor de 10 corridas de 600 s).
* **S2005**: Stoyan et al. (2005), mínimo local probado (Ex1 27.00, Ex2 30.917, Ex3 45.86).

## Instancias clásicas

| Instancia | Ítems | Mejor conocida | Fuente | BSG_PLP (30 s) | Diferencia |
|---|---|---|---|---|---|
| Stoyan 2005 Example1 | 7 | 17.46 (óptimo) | T2025 | 17.84 | +2.2% |
| Stoyan 2005 Example2 | 12 | 19.01 | E2009 | 20.30 | +6.8% |
| Stoyan 2005 Example3 | 25 | 29.76 | LF2023 (VNS) | 34.15 | +14.8% |
| Stoyan 2004 Example1 | 80 | — | — | 60.56 | — |
| Stoyan 2004 Example2 | 20 | 28.47 | LF2023 (TS/VNS) | 36.00 | +26.4% |
| Stoyan 2004 Example3 | 30 | 41.30 | LF2023 (VNS) | 58.38 | +41.4% |
| Stoyan 2004 Example4 | 40 | 54.83 | LF2023 (VNS) | 73.47 | +34.0% |
| Stoyan 2004 Example5 | 50 | 72.33 | LF2023 (VNS) | 90.68 | +25.4% |
| Egeblad Merged1–5 | 15–75 | 16.99 / 21.97 / 24.04 / 26.47 / 29.24 | E2009, LF2023 | sin descomposición | — |

Otras referencias para Stoyan 2004 Example2–5: Stoyan et al. (2004) 32 / 49 / 63.21 / 78.7 y HAPE3D (Liu et al. 2015)
36.9 / 55.6 / 71.8 / 92.

## Instancias de Tollenaere et al. (2025)

Resumen: BSG_PLP (10 s) mejora la mejor solución del MILP (1 h) en 10 de 66 instancias, la iguala en 20 y es
peor en 36. En las 27 instancias con óptimo probado, BSG_PLP lo alcanza en 14 y en las otras 13 queda en promedio
3.4% sobre el óptimo (máximo 11.4%, TETRAHEDRONS 6).

| Serie | Ítems | MILP: mejor altura | Óptimo | BSG_PLP (10 s) | Diferencia |
|---|---|---|---|---|---|
| DIAMONDS | 2 | 9.56 | sí | 9.56 | +0.0% |
| DIAMONDS | 3 | 13.15 | sí | 13.15 | +0.0% |
| DIAMONDS | 4 | 17.85 | sí | 17.85 | +0.0% |
| DIAMONDS | 5 | 10.69 | sí | 11.30 | +5.7% |
| DIAMONDS | 6 | 12.14 | no | 12.73 | +4.9% |
| DIAMONDS | 7 | 15.02 | no | 14.19 | -5.5% |
| DIAMONDS | 8 | 16.23 | no | 15.28 | -5.9% |
| DIAMONDS | 9 | 17.47 | no | 18.10 | +3.6% |
| DIAMONDS | 10 | 18.87 | no | 18.32 | -2.9% |
| STOYAN 2004 | 2 | 16.00 | sí | 16.00 | +0.0% |
| STOYAN 2004 | 3 | 19.30 | sí | 20.22 | +4.8% |
| STOYAN 2004 | 4 | 27.53 | sí | 27.70 | +0.6% |
| STOYAN 2004 | 5 | 28.00 | sí | 28.00 | +0.0% |
| STOYAN 2004 | 6 | 28.00 | sí | 28.37 | +1.3% |
| STOYAN 2004 | 7 | 31.61 | sí | 31.61 | +0.0% |
| STOYAN 2004 | 8 | 34.59 | sí | 36.21 | +4.7% |
| STOYAN 2004 | 9 | 40.92 | sí | 42.00 | +2.6% |
| STOYAN 2004 | 10 | 30.19 | sí | 31.00 | +2.7% |
| STOYAN 2004 | 11 | 32.31 | no | 31.79 | -1.6% |
| STOYAN 2004 | 12 | 32.78 | no | 33.09 | +0.9% |
| STOYAN 2004 | 13 | 36.87 | no | 37.00 | +0.4% |
| STOYAN 2004 | 14 | 40.05 | no | 40.63 | +1.4% |
| STOYAN 2004 | 15 | 32.70 | no | 34.00 | +4.0% |
| STOYAN 2004 | 16 | 33.35 | no | 35.94 | +7.8% |
| STOYAN 2004 | 17 | 36.75 | no | 38.00 | +3.4% |
| STOYAN 2004 | 18 | 37.60 | no | 38.00 | +1.1% |
| STOYAN 2004 | 19 | 40.00 | no | 41.26 | +3.1% |
| STOYAN 2004 | 20 | 44.00 | no | 44.93 | +2.1% |
| STOYAN 2005 | 2 | 12.00 | sí | 12.00 | +0.0% |
| STOYAN 2005 | 3 | 16.73 | sí | 17.00 | +1.6% |
| STOYAN 2005 | 4 | 17.00 | sí | 17.00 | +0.0% |
| STOYAN 2005 | 5 | 15.00 | sí | 15.00 | +0.0% |
| STOYAN 2005 | 6 | 16.00 | sí | 16.00 | +0.0% |
| STOYAN 2005 | 7 | 17.46 | sí | 17.84 | +2.2% |
| STOYAN 2005 | 8 | 22.71 | sí | 23.00 | +1.3% |
| STOYAN 2005 | 9 | 24.93 | sí | 25.23 | +1.2% |
| STOYAN 2005 | 10 | 26.05 | sí | 27.00 | +3.6% |
| STOYAN 2005 | 11 | 18.00 | no | 20.00 | +11.1% |
| STOYAN 2005 | 12 | 20.07 | no | 21.25 | +5.9% |
| STOYAN 2005 | 13 | 20.76 | no | 21.28 | +2.5% |
| STOYAN 2005 | 14 | 21.93 | no | 21.47 | -2.1% |
| STOYAN 2005 | 15 | 23.00 | no | 26.00 | +13.0% |
| STOYAN 2005 | 16 | 24.90 | no | 27.41 | +10.1% |
| STOYAN 2005 | 17 | 26.00 | no | 27.31 | +5.0% |
| STOYAN 2005 | 18 | 27.02 | no | 28.97 | +7.2% |
| STOYAN 2005 | 19 | 28.38 | no | 32.22 | +13.5% |
| STOYAN 2005 | 20 | 29.90 | no | 32.82 | +9.8% |
| TETRAHEDRONS | 2 | 12.01 | sí | 12.01 | +0.0% |
| TETRAHEDRONS | 3 | 17.01 | sí | 17.01 | +0.0% |
| TETRAHEDRONS | 4 | 19.51 | sí | 19.51 | +0.0% |
| TETRAHEDRONS | 5 | 24.51 | sí | 24.51 | +0.0% |
| TETRAHEDRONS | 6 | 15.18 | sí | 16.91 | +11.4% |
| TETRAHEDRONS | 7 | 18.45 | no | 18.45 | +0.0% |
| TETRAHEDRONS | 8 | 20.00 | no | 20.00 | +0.0% |
| TETRAHEDRONS | 9 | 20.36 | no | 20.36 | +0.0% |
| TETRAHEDRONS | 10 | 22.68 | no | 22.68 | +0.0% |
| TETRAHEDRONS | 11 | 24.86 | no | 25.12 | +1.0% |
| TETRAHEDRONS | 12 | 27.59 | no | 27.86 | +1.0% |
| TETRAHEDRONS | 13 | 29.91 | no | 29.59 | -1.1% |
| TETRAHEDRONS | 14 | 30.36 | no | 30.36 | +0.0% |
| TETRAHEDRONS | 15 | 33.44 | no | 32.09 | -4.0% |
| TETRAHEDRONS | 16 | 34.59 | no | 34.59 | +0.0% |
| TETRAHEDRONS | 17 | 37.52 | no | 37.68 | +0.4% |
| TETRAHEDRONS | 18 | 40.36 | no | 38.22 | -5.3% |
| TETRAHEDRONS | 19 | 40.54 | no | 40.36 | -0.4% |
| TETRAHEDRONS | 20 | 43.81 | no | 42.21 | -3.7% |
