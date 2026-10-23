# Ablación: ¿qué componentes aportan?

Instancias difíciles (Stoyan 2004 Example1–5, Stoyan 2005 Example2–3), 120 s, semillas 1–3, 4 corridas en
paralelo. `actual`: parámetros por defecto (BSG inicial 30 % del tiempo, pseudobloques, separación por los ejes y
luego por LP, GLS multiplicativo, 500 pasadas, reparación con BSG (fase B) tras 30 kicks sin mejora, compactación
por LP con ibex/SoPlex).

| Config. | Opciones |
|---|---|
| actual | — |
| mínima | `--sep_method axis --blocks 0 --kicks 1000000 --strip_time 0.01` (greedy inicial, sin pseudobloques, sin LP de separación, sin fase B) |
| mínima sin LP | mínima + `--no_compact` (sin ibex: sin compactación por LP) |
| sin fase B | `--kicks 1000000` |
| sin LP de sep. | `--sep_method axis` |
| sin pseudobloques | `--blocks 0` |
| BSG 1 % | `--strip_time 0.01` |

## Media (mejor) de 3 semillas

| Instancia | Ref. | actual | mínima | mínima sin LP | sin fase B | sin LP de sep. | sin pseudobloques | BSG 1 % |
|---|---|---|---|---|---|---|---|---|
| S2004 Ex1 | — | 52.28 (52.00) | 52.92 (52.25) | 52.67 (52.55) | 52.20 (51.74) | 54.02 (53.74) | 52.08 (51.40) | 51.99 (51.41) |
| S2004 Ex2 | 28.47 | 29.00 (29.00) | 29.00 (29.00) | 29.00 (29.00) | 29.00 (29.00) | 29.00 (29.00) | 29.00 (29.00) | 29.00 (29.00) |
| S2004 Ex3 | 41.30 | 39.17 (38.93) | 39.80 (38.93) | 40.06 (39.50) | 39.31 (38.93) | 40.37 (40.00) | 40.56 (39.86) | 39.76 (39.00) |
| S2004 Ex4 | 54.83 | 54.60 (53.39) | 53.33 (**52.64**) | 53.83 (53.72) | 54.69 (53.39) | 54.01 (53.60) | 55.96 (55.87) | 54.52 (52.84) |
| S2004 Ex5 | 72.33 | 67.75 (66.10) | 65.60 (**64.20**) | 68.09 (67.29) | 67.22 (65.75) | 68.04 (66.66) | 68.37 (66.78) | 66.72 (65.40) |
| S2005 Ex2 | 19.01 | 19.13 (19.13) | 19.13 (19.13) | 19.16 (19.15) | 19.13 (19.13) | 19.13 (19.13) | 19.13 (19.13) | 19.13 (19.13) |
| S2005 Ex3 | 29.76 | 27.59 (27.33) | 27.61 (27.24) | 27.43 (27.21) | 27.77 (27.45) | 27.66 (27.13) | 27.79 (27.58) | 27.64 (27.45) |

| Brecha media | actual | mínima | mínima sin LP | sin fase B | sin LP de sep. | sin pseudobloques | BSG 1 % |
|---|---|---|---|---|---|---|---|
| Stoyan 2004 (Ex2–5) | -2.5% | **-3.5%** | -2.2% | -2.6% | -2.0% | -0.8% | -2.5% |
| Stoyan 2005 (Ex2–3) | -3.3% | -3.3% | **-3.5%** | -3.0% | -3.2% | -3.0% | -3.3% |
| Mejor / peor que actual (media) | | 2 / 3 | 2 / 4 | 2 / 3 | 1 / 4 | 1 / 4 | 3 / 2 |

## Conclusiones

* La versión mínima (greedy inicial → oscilación con separación por los ejes, GLS multiplicativo y kicks →
  compactación por LP) rinde igual que la completa; las diferencias (~1 %) están dentro del ruido de 3 semillas.
  El BSG inicial, la reparación con BSG, los pseudobloques y el LP de separación no son necesarios.
* Quitados de a uno, los pseudobloques y el LP de separación parecen aportar algo en Stoyan 2004, pero el efecto
  desaparece al quitar todo junto: no hay evidencia de que aporten.
* Sin ibex (sin compactación por LP) el algoritmo funciona casi igual (-2.2 % / -3.5 %); la compactación aporta
  ~1 % en Stoyan 2004. Antes de esta prueba, sin LP no se ejecutaba la fase de oscilación (corregido).
* Nuevas mejores soluciones (mínima, semilla 1; verificadas con `PLP_check` y `tools/check_solution.py`,
  eps 1e-9): S2004 Ex4 **52.64**, Ex5 **64.20**.
