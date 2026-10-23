# Separación por LP

Separación por LP (commit `9ec385e`) contra la misma versión sin separación (`--no_separate`). 3 semillas, 30 s (clásicas) y 10 s (Tollenaere 2025), parámetros por defecto (Optuna) y pseudobloques. Todas las soluciones fueron verificadas.

Tollenaere 2025 (mejor de 3 semillas): supera la mejor solución del MILP (1 h) en 15 instancias (14 sin separación) y alcanza 17 de los 27 óptimos probados (15 sin separación).

3 semillas, 30 s (clásicas) y 10 s (Tollenaere 2025), parámetros por defecto (Optuna) y pseudobloques.

| Instancia | Ref. | nosep mejor | nosep media | sep mejor | sep media |
|---|---|---|---|---|---|
| Stoyan et al. 2004 Example1 | — | 58.97 | 59.28 | 56.25 | 56.70 |
| Stoyan et al. 2004 Example2 | 28.47 | 36.00 | 37.16 | 35.80 | 37.07 |
| Stoyan et al. 2004 Example3 | 41.30 | 51.98 | 52.58 | 48.04 | 49.16 |
| Stoyan et al. 2004 Example4 | 54.83 | 65.74 | 66.78 | 64.67 | 64.69 |
| Stoyan et al. 2004 Example5 | 72.33 | 81.10 | 81.98 | 78.29 | 80.12 |
| Stoyan et al. 2005 Example1 | 17.46 | 17.46 | 17.46 | 17.46 | 17.46 |
| Stoyan et al. 2005 Example2 | 19.01 | 21.00 | 21.00 | 20.50 | 20.50 |
| Stoyan et al. 2005 Example3 | 29.76 | 32.74 | 32.75 | 30.92 | 32.34 |
| Grupo | n | nosep: brecha mejor | nosep: brecha media | sep: brecha mejor | sep: brecha media |
| Stoyan 2004 | 4 | +21.1% | +23.2% | +17.1% | +19.5% |
| Stoyan 2005 | 3 | +6.8% | +6.8% | +3.9% | +5.5% |
| T2025/DIAMONDS | 9 | +1.7% | +2.3% | +2.4% | +2.4% |
| T2025/STOYAN 2004 | 19 | +1.4% | +1.9% | +1.0% | +1.5% |
| T2025/STOYAN 2005 | 19 | +4.0% | +4.7% | +2.9% | +3.9% |
| T2025/TETRAHEDRONS | 19 | -0.1% | -0.1% | -0.7% | -0.7% |
sep vs nosep (media por instancia): mejor en 26, peor en 5, igual en 43
