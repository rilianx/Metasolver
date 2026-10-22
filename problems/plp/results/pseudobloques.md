# Pseudobloques (`--blocks 30`) contra la versión base

Commit `71e2912`, 3 semillas por instancia (`--seed 1..3`), 30 s (Stoyan 2004/2005) y 10 s (Tollenaere 2025), ibex/SoPlex.
Ref.: mejor altura conocida (ver `comparacion.md`); para Tollenaere 2025, la mejor cota superior del MILP (1 h).
Brecha = (altura − Ref.)/Ref.; "mejor" es la mejor de las 3 semillas y "media" el promedio. Todas las soluciones fueron verificadas.

| Instancia | Ref. | base mejor | base media | b30 mejor | b30 media |
|---|---|---|---|---|---|
| Stoyan et al. 2004 Example1 | — | 60.26 | 62.06 | 62.35 | 63.36 |
| Stoyan et al. 2004 Example2 | 28.47 | 37.40 | 38.49 | 37.00 | 39.25 |
| Stoyan et al. 2004 Example3 | 41.30 | 53.84 | 55.58 | 62.26 | 63.24 |
| Stoyan et al. 2004 Example4 | 54.83 | 68.02 | 70.59 | 74.27 | 78.74 |
| Stoyan et al. 2004 Example5 | 72.33 | 91.09 | 93.69 | 95.00 | 98.36 |
| Stoyan et al. 2005 Example1 | 17.46 | 17.84 | 17.84 | 17.84 | 17.84 |
| Stoyan et al. 2005 Example2 | 19.01 | 20.70 | 20.78 | 20.28 | 20.50 |
| Stoyan et al. 2005 Example3 | 29.76 | 32.29 | 33.12 | 31.00 | 31.40 |

| Grupo | n | base: brecha mejor | base: brecha media | b30: brecha mejor | b30: brecha media |
|---|---|---|---|---|---|
| Stoyan 2004 | 4 | +27.9% | +32.0% | +36.9% | +42.6% |
| Stoyan 2005 | 3 | +6.5% | +7.6% | +4.3% | +5.2% |
| T2025/DIAMONDS | 9 | -0.5% | -0.4% | -0.3% | -0.3% |
| T2025/STOYAN 2004 | 19 | +1.6% | +2.3% | +3.7% | +3.9% |
| T2025/STOYAN 2005 | 19 | +3.9% | +5.1% | +3.2% | +3.9% |
| T2025/TETRAHEDRONS | 19 | -0.1% | -0.1% | -0.1% | -0.1% |

b30 vs base (media por instancia): mejor en 12, peor en 21, igual en 41

## Criterio por hueco en la envolvente convexa (`h30`, commit `d65aea2`) contra la versión base

Mismas condiciones (3 semillas). `h30`: `--blocks 30`, aceptación por hueco agregado ≤ 0.1 y evaluación por volumen por pieza.

| Instancia | Ref. | base mejor | base media | h30 mejor | h30 media |
|---|---|---|---|---|---|
| Stoyan et al. 2004 Example1 | — | 60.26 | 62.06 | 62.57 | 62.96 |
| Stoyan et al. 2004 Example2 | 28.47 | 37.40 | 38.49 | 36.00 | 37.92 |
| Stoyan et al. 2004 Example3 | 41.30 | 53.84 | 55.58 | 56.26 | 57.36 |
| Stoyan et al. 2004 Example4 | 54.83 | 68.02 | 70.59 | 69.41 | 72.76 |
| Stoyan et al. 2004 Example5 | 72.33 | 91.09 | 93.69 | 90.36 | 93.42 |
| Stoyan et al. 2005 Example1 | 17.46 | 17.84 | 17.84 | 17.84 | 17.84 |
| Stoyan et al. 2005 Example2 | 19.01 | 20.70 | 20.78 | 20.82 | 20.82 |
| Stoyan et al. 2005 Example3 | 29.76 | 32.29 | 33.12 | 33.32 | 33.81 |

| Grupo | n | base: brecha mejor | base: brecha media | h30: brecha mejor | h30: brecha media |
|---|---|---|---|---|---|
| Stoyan 2004 | 4 | +27.9% | +32.0% | +28.5% | +33.5% |
| Stoyan 2005 | 3 | +6.5% | +7.6% | +7.9% | +8.4% |
| T2025/DIAMONDS | 9 | -0.5% | -0.4% | -0.6% | -0.3% |
| T2025/STOYAN 2004 | 19 | +1.6% | +2.3% | +1.2% | +1.6% |
| T2025/STOYAN 2005 | 19 | +3.9% | +5.1% | +4.9% | +6.0% |
| T2025/TETRAHEDRONS | 19 | -0.1% | -0.1% | -0.1% | -0.1% |

h30 vs base (media por instancia): mejor en 11, peor en 16, igual en 47

h30 contra b30 (media por instancia): mejor en 23, peor en 12, igual en 39.

## Experimento 2×2: criterio de aceptación × volumen en la evaluación

Commit `9a8790b`, 3 semillas, `--blocks 30`. Configuraciones: `base` sin bloques; `bt` caja + total; `bp` caja + por pieza; `ht` hueco + total; `h30` hueco + por pieza; `b30` es la corrida anterior de caja + total (idéntica a `bt`).

| Grupo | n | base: brecha mejor | base: brecha media | bt: brecha mejor | bt: brecha media | bp: brecha mejor | bp: brecha media | ht: brecha mejor | ht: brecha media | h30: brecha mejor | h30: brecha media |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Stoyan 2004 | 4 | +27.9% | +32.0% | +32.0% | +40.3% | +33.0% | +37.4% | +27.7% | +28.5% | +28.5% | +33.5% |
| Stoyan 2005 | 3 | +6.5% | +7.6% | +4.3% | +5.6% | +5.1% | +7.3% | +6.0% | +6.8% | +7.9% | +8.4% |
| T2025/DIAMONDS | 9 | -0.5% | -0.4% | -0.3% | -0.2% | -0.4% | -0.2% | -0.5% | -0.1% | -0.6% | -0.3% |
| T2025/STOYAN 2004 | 19 | +1.6% | +2.3% | +3.4% | +3.9% | +1.7% | +2.0% | +1.2% | +1.5% | +1.2% | +1.6% |
| T2025/STOYAN 2005 | 19 | +3.9% | +5.1% | +3.1% | +3.8% | +3.3% | +4.1% | +3.3% | +4.5% | +4.9% | +6.0% |
| T2025/TETRAHEDRONS | 19 | -0.1% | -0.1% | -0.1% | -0.1% | -0.1% | -0.1% | -0.1% | -0.1% | -0.1% | -0.1% |


Contra `base` (media por instancia):

* bt vs base (media por instancia): mejor en 12, peor en 21, igual en 41
* bp vs base (media por instancia): mejor en 11, peor en 19, igual en 44
* ht vs base (media por instancia): mejor en 18, peor en 14, igual en 42
* h30 vs base (media por instancia): mejor en 11, peor en 16, igual en 47

Conclusión: `ht` (hueco + volumen total) es la mejor en conjunto y queda por defecto. El volumen por pieza era lo que perjudicaba a `h30`; el criterio por caja perjudica a las no convexas.
