# Resultados (/home/iaraya/bsg_poly/problems/plp/repro/out/runs)

- Model name: Intel(R) Xeon(R) CPU E5-2620 v4 @ 2.10GHz
- Socket(s): 2
- Core(s) per socket: 8
- Thread(s) per core: 1
- CPU max MHz: 3000.0000

## Tabla principal (120 s, 10 semillas)

| Instancia | n | Media | Mejor | Desv. | Infactibles | Certificadas |
|---|---|---|---|---|---|---|
| Stoyan et al. 2005/Example1 | 10 | 17.47 | 17.46 | 0.00 | 0 | 10/10 |
| Stoyan et al. 2005/Example2 | 10 | 19.13 | 19.13 | 0.00 | 0 | 10/10 |
| Stoyan et al. 2005/Example3 | 10 | 27.56 | 27.03 | 0.35 | 0 | 9/10 |
| Stoyan et al. 2004/Example1 | 10 | 53.60 | 52.65 | 0.69 | 0 | 10/10 |
| Stoyan et al. 2004/Example2 | 10 | 29.00 | 29.00 | 0.00 | 0 | 10/10 |
| Stoyan et al. 2004/Example3 | 10 | 39.88 | 39.46 | 0.27 | 0 | 10/10 |
| Stoyan et al. 2004/Example4 | 10 | 53.07 | 50.95 | 1.38 | 0 | 10/10 |
| Stoyan et al. 2004/Example5 | 10 | 66.67 | 65.04 | 1.05 | 0 | 10/10 |
| Egeblad et al. 2009/Merged1 | 10 | 16.67 | 16.57 | 0.05 | 0 | 10/10 |
| Egeblad et al. 2009/Merged2 | 10 | 20.95 | 20.29 | 0.22 | 0 | 10/10 |
| Egeblad et al. 2009/Merged3 | 10 | 22.64 | 22.00 | 0.31 | 0 | 10/10 |
| Egeblad et al. 2009/Merged4 | 10 | 25.03 | 24.20 | 0.44 | 0 | 10/10 |
| Egeblad et al. 2009/Merged5 | 10 | 27.69 | 27.00 | 0.37 | 0 | 10/10 |

Certificadas en total: 129 de 130

## Ablacion (brecha media respecto de la referencia; Stoyan 2004 Ex1: altura media)

| Variante | Stoyan 2004 | Stoyan 2005 | S2004 Ex1 | n |
|---|---|---|---|---|
| Propuesto | -3.1% | -1.9% | 53.60 | 10 |
| Sin LP | -2.9% | -2.0% | 53.08 | 10 |
| Pesos aditivos | -1.7% | -1.0% | 53.86 | 10 |
| Kick: solo reinsercion | -3.6% | -1.9% | 53.50 | 10 |
| 5x mas lento (24 s) | -0.4% | -0.9% | 55.22 | 10 |
| Sin Improve | -3.1% | -2.2% | 53.11 | 10 |
| Greedy: pieza mas grande primero | -2.6% | -2.1% | 53.33 | 10 |
| Descend sin termino de pared (sigma = 0) | -2.8% | -2.0% | 53.05 | 10 |
| Minima: sin LP, sin Improve, greedy por volumen, sigma = 0 | -2.6% | -1.7% | 53.17 | 10 |
| Simple: sin LP, sin Improve, sigma = 0 (greedy actual) | -2.7% | -1.8% | 53.25 | 10 |

## Merged: utilizacion media en el tiempo (3DNest a 600 s: 43.0, 42.9, 43.3, 42.0, 41.0 %)

| Instancia | 1 s | 10 s | 60 s | 120 s |
|---|---|---|---|---|
| Merged1 | 42.6 | 44.3 | 45.0 | 45.0 |
| Merged2 | 42.2 | 43.8 | 45.0 | 45.1 |
| Merged3 | 38.9 | 44.8 | 47.0 | 47.8 |
| Merged4 | 38.1 | 44.6 | 46.9 | 47.6 |
| Merged5 | 37.2 | 43.3 | 45.7 | 46.3 |

## Limites de iteraciones (todas las corridas)

- Compact: 0 de 9590 llamadas alcanzan el limite de 20 iteraciones (maximo usado: 20)
- LPCompact: 0 de 23811 llamadas alcanzan el limite de 30 iteraciones (maximo usado: 20)
