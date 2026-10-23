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

## Oscilación estratégica y kicks (commit `8c17470`) contra la separación sola

Mismas condiciones (3 semillas). Tollenaere 2025 (mejor de 3 semillas): supera la mejor solución del MILP (1 h) en 27 de 66 instancias (15 con la separación sola) y alcanza 19 de los 27 óptimos probados (17).

| Instancia | Ref. | sep mejor | sep media | osc mejor | osc media |
|---|---|---|---|---|---|
| Stoyan et al. 2004 Example1 | — | 56.25 | 56.70 | 54.41 | 55.06 |
| Stoyan et al. 2004 Example2 | 28.47 | 35.80 | 37.07 | 31.13 | 31.89 |
| Stoyan et al. 2004 Example3 | 41.30 | 48.04 | 49.16 | 46.34 | 47.55 |
| Stoyan et al. 2004 Example4 | 54.83 | 64.67 | 64.69 | 62.00 | 63.79 |
| Stoyan et al. 2004 Example5 | 72.33 | 78.29 | 80.12 | 74.81 | 75.99 |
| Stoyan et al. 2005 Example1 | 17.46 | 17.46 | 17.46 | 17.46 | 17.46 |
| Stoyan et al. 2005 Example2 | 19.01 | 20.50 | 20.50 | 20.50 | 20.50 |
| Stoyan et al. 2005 Example3 | 29.76 | 30.92 | 32.34 | 29.86 | 29.95 |
| Grupo | n | sep: brecha mejor | sep: brecha media | osc: brecha mejor | osc: brecha media |
|---|---|---|---|---|---|
| Stoyan 2004 | 4 | +17.1% | +19.5% | +9.5% | +12.1% |
| Stoyan 2005 | 3 | +3.9% | +5.5% | +2.7% | +2.8% |
| T2025/DIAMONDS | 9 | +2.4% | +2.4% | +2.4% | +3.1% |
| T2025/STOYAN 2004 | 19 | +1.0% | +1.5% | -0.6% | -0.1% |
| T2025/STOYAN 2005 | 19 | +2.9% | +3.9% | -0.4% | +0.3% |
| T2025/TETRAHEDRONS | 19 | -0.7% | -0.7% | -0.7% | -0.7% |
osc vs sep (media por instancia): mejor en 32, peor en 4, igual en 38
[exited with code 0]
