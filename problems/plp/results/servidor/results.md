# Resultados (/home/iaraya/bsg_poly/problems/plp/repro/out/runs)

- Model name: Intel(R) Xeon(R) CPU E5-2620 v4 @ 2.10GHz
- Socket(s): 2
- Core(s) per socket: 8
- Thread(s) per core: 1
- CPU max MHz: 3000.0000

## Tabla principal (120 s, 10 semillas)

| Instancia | n | Media | Mejor | Desv. | Infactibles | Certificadas |
|---|---|---|---|---|---|---|
| Stoyan et al. 2005/Example1 | 10 | 17.47 | 17.46 | 0.01 | 0 | 2/10 |
| Stoyan et al. 2005/Example2 | 10 | 19.15 | 19.13 | 0.01 | 0 | 0/10 |
| Stoyan et al. 2005/Example3 | 10 | 27.67 | 27.29 | 0.27 | 0 | 0/10 |
| Stoyan et al. 2004/Example1 | 10 | 53.23 | 51.53 | 0.92 | 0 | 0/10 |
| Stoyan et al. 2004/Example2 | 10 | 29.00 | 29.00 | 0.00 | 0 | 0/10 |
| Stoyan et al. 2004/Example3 | 10 | 40.08 | 39.32 | 0.34 | 0 | 0/10 |
| Stoyan et al. 2004/Example4 | 10 | 53.54 | 52.10 | 0.90 | 0 | 0/10 |
| Stoyan et al. 2004/Example5 | 10 | 67.40 | 65.88 | 1.05 | 0 | 0/10 |
| Egeblad et al. 2009/Merged1 | 10 | 16.66 | 16.56 | 0.07 | 0 | 0/10 |
| Egeblad et al. 2009/Merged2 | 10 | 21.06 | 21.01 | 0.04 | 0 | 0/10 |
| Egeblad et al. 2009/Merged3 | 10 | 22.45 | 22.01 | 0.35 | 0 | 0/10 |
| Egeblad et al. 2009/Merged4 | 10 | 25.37 | 24.40 | 0.59 | 0 | 0/10 |
| Egeblad et al. 2009/Merged5 | 10 | 28.01 | 27.60 | 0.14 | 0 | 0/10 |
| Liu et al. 2015/Experiment1 | 10 | 30.15 | 29.98 | 0.11 | 0 | 0/10 |

Certificadas en total: 2 de 140

## Ablacion (brecha media respecto de la referencia; Stoyan 2004 Ex1: altura media)

| Variante | Stoyan 2004 | Stoyan 2005 | S2004 Ex1 | n |
|---|---|---|---|---|
| Propuesto | -2.6% | -1.6% | 53.23 | 10 |
| 5x mas lento (24 s) | +0.3% | -0.5% | 54.76 | 10 |
| + compactacion por LP | -3.1% | -2.2% | 53.11 | 10 |
| + Improve | -2.9% | -2.0% | 53.08 | 10 |
| Pesos aditivos | -1.4% | -0.5% | 53.22 | 10 |
| Descend sin termino de pared (sigma = 0) | -2.7% | -1.8% | 53.25 | 10 |
| Greedy: pieza mas grande primero | -2.4% | -1.5% | 53.29 | 10 |
| Kick: ninguno (solo reinicio) | -2.7% | -1.7% | 53.84 | 10 |
| Kick: mixto (reinsercion o aleatorio) | -2.8% | -1.7% | 53.43 | 10 |
| Kick: solo reinsercion | -2.6% | -1.9% | 53.07 | 10 |
| Kick: pieza superior, aleatoria | -2.4% | -1.7% | 53.03 | 10 |
| Kick: pieza superior, menor penetracion de 20 | -2.8% | -2.1% | 53.41 | 10 |
| rho = 0.5 % | -2.9% | -2.2% | 53.59 | 10 |
| rho = 2 % | -2.5% | -1.6% | 53.08 | 10 |
| 100 pasadas | -1.1% | -1.4% | 55.10 | 10 |
| 2000 pasadas | -2.0% | -1.5% | 53.12 | 10 |

## Merged: utilizacion media en el tiempo (3DNest a 600 s: 43.0, 42.9, 43.3, 42.0, 41.0 %)

| Instancia | 1 s | 10 s | 60 s | 120 s |
|---|---|---|---|---|
| Merged1 | 41.9 | 44.2 | 44.9 | 45.0 |
| Merged2 | 42.4 | 43.8 | 44.8 | 44.9 |
| Merged3 | 40.3 | 45.1 | 47.5 | 48.2 |
| Merged4 | 39.2 | 43.3 | 45.9 | 47.0 |
| Merged5 | 39.0 | 43.3 | 45.7 | 45.8 |

## Limites de iteraciones (todas las corridas)

- Compact: 0 de 1486 llamadas alcanzan el limite de 20 iteraciones (maximo usado: 5)
- LPCompact: 0 de 2984 llamadas alcanzan el limite de 30 iteraciones (maximo usado: 8)

## Tollenaere et al. 2025 (30 s, 10 semillas)

| Serie | Instancias | Optimos (T2025) | Optimos alcanzados | Abiertas | Abiertas mejoradas | Brecha media (media) | Brecha media (mejor) | Infactibles |
|---|---|---|---|---|---|---|---|---|
| DIAMONDS | 9 | 4 | 4 | 5 | 5 | -2.32% | -2.67% | 0 |
| STOYAN 2004 | 19 | 9 | 6 | 10 | 9 | -2.52% | -3.03% | 0 |
| STOYAN 2005 | 19 | 9 | 6 | 10 | 9 | -2.37% | -2.93% | 0 |
| TETRAHEDRONS | 19 | 5 | 3 | 14 | 9 | -1.21% | -1.29% | 0 |
| Total | 66 | 27 | 19 | 39 | 32 | | | 0 |

## Chess (Lamas: mejor 131, media 135.1)

| Descomposicion | n | Media | Mejor | Infactibles |
|---|---|---|---|---|
| fina (t=0.05), 1200 s | 5 | 149.81 | 144.87 | 0 |
| gruesa (t=0.2, k-DOP), 1200 s | 5 | 153.93 | 151.16 | 0 |
| gruesa 900 s + fina 300 s | 5 | 144.54 | 138.29 | 0 |
