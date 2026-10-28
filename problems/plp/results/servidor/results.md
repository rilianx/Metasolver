# Resultados (/home/iaraya/bsg_poly/problems/plp/repro/out/runs)

- Model name: Intel(R) Xeon(R) CPU E5-2620 v4 @ 2.10GHz
- Socket(s): 2
- Core(s) per socket: 8
- Thread(s) per core: 1
- CPU max MHz: 3000.0000

## Tabla principal (120 s, 10 semillas)

| Instancia | n | Media | Mejor | Desv. | Infactibles | Certificadas |
|---|---|---|---|---|---|---|
| Stoyan et al. 2005/Example1 | 10 | 17.47 | 17.46 | 0.01 | 0 | 0/10 |
| Stoyan et al. 2005/Example2 | 10 | 19.16 | 19.14 | 0.01 | 0 | 0/10 |
| Stoyan et al. 2005/Example3 | 10 | 27.45 | 27.05 | 0.39 | 0 | 0/10 |
| Stoyan et al. 2004/Example1 | 10 | 53.08 | 52.10 | 0.61 | 0 | 0/10 |
| Stoyan et al. 2004/Example2 | 10 | 29.00 | 29.00 | 0.00 | 0 | 0/10 |
| Stoyan et al. 2004/Example3 | 10 | 40.00 | 39.13 | 0.49 | 0 | 0/10 |
| Stoyan et al. 2004/Example4 | 10 | 53.32 | 52.15 | 1.00 | 0 | 0/10 |
| Stoyan et al. 2004/Example5 | 10 | 66.96 | 65.13 | 1.04 | 0 | 0/10 |
| Egeblad et al. 2009/Merged1 | 10 | 16.70 | 16.56 | 0.07 | 0 | 0/10 |
| Egeblad et al. 2009/Merged2 | 10 | 21.10 | 21.01 | 0.11 | 0 | 0/10 |
| Egeblad et al. 2009/Merged3 | 10 | 22.60 | 22.01 | 0.37 | 0 | 0/10 |
| Egeblad et al. 2009/Merged4 | 10 | 24.93 | 24.12 | 0.63 | 0 | 0/10 |
| Egeblad et al. 2009/Merged5 | 10 | 27.93 | 27.07 | 0.30 | 0 | 0/10 |
| Liu et al. 2015/Experiment1 | 10 | 30.22 | 30.00 | 0.22 | 0 | 0/10 |

Certificadas en total: 0 de 140

## Ablacion (brecha media respecto de la referencia; Stoyan 2004 Ex1: altura media)

| Variante | Stoyan 2004 | Stoyan 2005 | S2004 Ex1 | n |
|---|---|---|---|---|
| Propuesto | -2.9% | -2.0% | 53.08 | 10 |
| 5x mas lento (24 s) | -0.1% | -1.2% | 54.64 | 10 |
| + compactacion por LP | -3.1% | -1.9% | 53.60 | 10 |
| Sin Improve | -2.6% | -1.6% | 53.26 | 10 |
| Pesos aditivos | -1.4% | -0.8% | 53.53 | 10 |
| Descend sin termino de pared (sigma = 0) | -2.3% | -1.7% | 53.42 | 10 |
| Greedy: pieza mas grande primero | -2.9% | -1.4% | 53.49 | 10 |
| Kick: ninguno (solo reinicio) | -2.6% | -2.1% | 53.49 | 10 |
| Kick: mixto (reinsercion o aleatorio) | -3.3% | -1.9% | 53.30 | 10 |
| Kick: solo reinsercion | -2.8% | -1.7% | 53.07 | 10 |
| Kick: pieza superior, aleatoria | -2.5% | -1.6% | 53.05 | 10 |
| Kick: pieza superior, menor penetracion de 20 | -2.6% | -1.9% | 52.65 | 10 |
| rho = 0.5 % | -2.8% | -2.1% | 53.17 | 10 |
| rho = 2 % | -2.2% | -1.5% | 52.72 | 10 |
| 100 pasadas | -1.1% | -1.1% | 54.98 | 10 |
| 2000 pasadas | -2.0% | -1.2% | 52.89 | 10 |

## Merged: utilizacion media en el tiempo (3DNest a 600 s: 43.0, 42.9, 43.3, 42.0, 41.0 %)

| Instancia | 1 s | 10 s | 60 s | 120 s |
|---|---|---|---|---|
| Merged1 | 41.2 | 44.1 | 44.7 | 44.9 |
| Merged2 | 41.9 | 43.1 | 44.7 | 44.8 |
| Merged3 | 40.5 | 44.9 | 47.5 | 47.9 |
| Merged4 | 39.1 | 43.8 | 46.8 | 47.8 |
| Merged5 | 39.0 | 43.1 | 45.9 | 45.9 |

## Limites de iteraciones (todas las corridas)

- Compact: 0 de 1323 llamadas alcanzan el limite de 20 iteraciones (maximo usado: 13)
- LPCompact: 0 de 3552 llamadas alcanzan el limite de 30 iteraciones (maximo usado: 8)

## Tollenaere et al. 2025 (30 s, 10 semillas)

| Serie | Instancias | Optimos (T2025) | Optimos alcanzados | Abiertas | Abiertas mejoradas | Brecha media (media) | Brecha media (mejor) | Infactibles |
|---|---|---|---|---|---|---|---|---|
| DIAMONDS | 9 | 4 | 4 | 5 | 5 | -2.47% | -2.91% | 0 |
| STOYAN 2004 | 19 | 9 | 8 | 10 | 9 | -2.50% | -3.02% | 0 |
| STOYAN 2005 | 19 | 9 | 7 | 10 | 9 | -2.43% | -2.92% | 0 |
| TETRAHEDRONS | 19 | 5 | 5 | 14 | 9 | -1.23% | -1.30% | 0 |
| Total | 66 | 27 | 24 | 39 | 32 | | | 0 |

## Chess (Lamas: mejor 131, media 135.1)

| Descomposicion | n | Media | Mejor | Infactibles |
|---|---|---|---|---|
| fina (t=0.05), 1200 s | 5 | 151.17 | 146.25 | 0 |
| gruesa (t=0.2, k-DOP), 1200 s | 5 | 154.99 | 149.91 | 0 |
| gruesa 900 s + fina 300 s | 5 | 148.36 | 138.55 | 0 |
