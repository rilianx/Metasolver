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
| Stoyan et al. 2005/Example3 | 10 | 27.50 | 27.22 | 0.20 | 0 | 9/10 |
| Stoyan et al. 2004/Example1 | 10 | 53.41 | 51.22 | 0.87 | 0 | 10/10 |
| Stoyan et al. 2004/Example2 | 10 | 29.00 | 29.00 | 0.00 | 0 | 10/10 |
| Stoyan et al. 2004/Example3 | 10 | 39.90 | 39.00 | 0.38 | 0 | 10/10 |
| Stoyan et al. 2004/Example4 | 10 | 53.60 | 52.35 | 1.08 | 0 | 10/10 |
| Stoyan et al. 2004/Example5 | 10 | 66.81 | 64.97 | 1.20 | 0 | 10/10 |
| Egeblad et al. 2009/Merged1 | 10 | 16.54 | 16.04 | 0.24 | 0 | 10/10 |
| Egeblad et al. 2009/Merged2 | 10 | 21.04 | 21.00 | 0.04 | 0 | 10/10 |
| Egeblad et al. 2009/Merged3 | 10 | 22.53 | 22.02 | 0.37 | 0 | 10/10 |
| Egeblad et al. 2009/Merged4 | 10 | 25.31 | 24.40 | 0.65 | 0 | 10/10 |
| Egeblad et al. 2009/Merged5 | 10 | 27.88 | 26.44 | 0.48 | 0 | 10/10 |
| Liu et al. 2015/Experiment1 | 10 | 30.11 | 29.52 | 0.25 | 0 | 10/10 |

Certificadas en total: 139 de 140

## Ablacion (brecha media respecto de la referencia; Stoyan 2004 Ex1: altura media)

| Variante | Stoyan 2004 | Stoyan 2005 | S2004 Ex1 | n |
|---|---|---|---|---|
| Propuesto | -2.8% | -2.0% | 53.41 | 10 |
| Sin LP | -3.3% | -1.9% | 53.30 | 10 |
| Pesos aditivos | -1.3% | -1.5% | 53.15 | 10 |
| Solo reinsercion | -3.5% | -1.9% | 53.50 | 10 |
| 5x mas lento (24 s) | -0.1% | -0.9% | 54.82 | 10 |

## Merged: utilizacion media en el tiempo (3DNest a 600 s: 43.0, 42.9, 43.3, 42.0, 41.0 %)

| Instancia | 1 s | 10 s | 60 s | 120 s |
|---|---|---|---|---|
| Merged1 | 42.7 | 44.3 | 45.1 | 45.3 |
| Merged2 | 42.2 | 43.4 | 44.7 | 44.9 |
| Merged3 | 38.9 | 45.1 | 47.1 | 48.0 |
| Merged4 | 38.1 | 44.7 | 46.7 | 47.1 |
| Merged5 | 37.4 | 43.2 | 45.5 | 46.0 |

## Tollenaere et al. 2025 (30 s, 10 semillas)

| Serie | Instancias | Optimos (T2025) | Optimos alcanzados | Abiertas | Abiertas mejoradas | Brecha media (media) | Brecha media (mejor) | Infactibles |
|---|---|---|---|---|---|---|---|---|
| DIAMONDS | 9 | 4 | 4 | 5 | 5 | -2.48% | -2.95% | 0 |
| STOYAN 2004 | 19 | 9 | 9 | 10 | 9 | -2.59% | -2.97% | 0 |
| STOYAN 2005 | 19 | 9 | 8 | 10 | 9 | -2.41% | -2.89% | 0 |
| TETRAHEDRONS | 19 | 5 | 5 | 14 | 9 | -1.26% | -1.30% | 0 |
| Total | 66 | 27 | 26 | 39 | 32 | | | 0 |

## Chess (Lamas: mejor 131, media 135.1)

| Descomposicion | n | Media | Mejor | Infactibles |
|---|---|---|---|---|
| fina (t=0.05), 1200 s | 5 | 150.49 | 143.59 | 0 |
| gruesa (t=0.2, k-DOP), 1200 s | 5 | 152.49 | 151.23 | 0 |
| gruesa 900 s + fina 300 s | 5 | 144.96 | 139.24 | 0 |
