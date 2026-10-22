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
