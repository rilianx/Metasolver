# Ideas de los papers: GLS multiplicativo, strikes, exploración, intercambio y compresión

Instancias difíciles (Stoyan 2004 Example1–5, Stoyan 2005 Example2–3), 120 s, semillas 1–3, 4 corridas en
paralelo, ibex/SoPlex. `base` es la versión anterior (`--sep_method both`, pesos +1, kick por reinserción).
Brecha respecto de la mejor publicada (Stoyan 2004 Example1 no tiene referencia).

| Config. | Opciones | Origen |
|---|---|---|
| base | — | |
| mult | `--gls mult` | pesos multiplicativos según la severidad, decaimiento ×0.95 (Gardeyn et al. 2025, sparrow) |
| strk | `--strikes 5` | volver a la mejor disposición solapada; 3 strikes terminan la separación (sparrow) |
| expl | `--sep_step 0.005 --sep_inc 0` | bajadas de 0.5 % y sin subir la altura al fallar (exploración de sparrow) |
| swap | `--kick swap` | intercambio de dos piezas de distinto tipo y volumen parecido (Imamichi et al. 2009; sparrow) |
| mix | `--kick mix` | intercambio o reinserción (1/2 cada uno) |
| comp | `--compress 0.2` | último 20 % del tiempo: bajadas de 0.05 % a 0.001 % desde la mejor solución (sparrow) |
| all | mult + strk + mix + comp | |
| ms | mult + swap | |
| mm | mult + mix | |

## Media (mejor) de 3 semillas

| Instancia | Ref. | base | mult | strk | expl | swap | mix | comp | all | ms | mm |
|---|---|---|---|---|---|---|---|---|---|---|---|
| S2004 Ex1 | — | 52.43 (52.01) | 52.52 (52.00) | 53.38 (53.11) | 54.51 (53.95) | 52.99 (52.94) | 53.05 (52.30) | 52.81 (52.42) | 53.31 (53.13) | 53.12 (52.80) | 53.14 (52.78) |
| S2004 Ex2 | 28.47 | 29.88 (29.00) | 29.00 (29.00) | 30.59 (29.60) | 29.63 (29.00) | 29.12 (29.00) | 29.23 (29.00) | 30.30 (29.69) | 29.54 (29.00) | 29.00 (29.00) | 29.00 (29.00) |
| S2004 Ex3 | 41.30 | 42.26 (41.64) | 42.12 (41.88) | 44.31 (42.51) | 44.01 (43.83) | 43.66 (42.25) | 42.86 (41.83) | 42.74 (41.54) | 43.98 (42.80) | 41.61 (**40.24**) | 41.55 (41.31) |
| S2004 Ex4 | 54.83 | 57.57 (56.00) | 55.18 (**53.33**) | 59.60 (59.02) | 57.71 (56.71) | 56.51 (**54.26**) | 57.23 (56.00) | 57.89 (56.00) | 58.60 (56.24) | 56.92 (56.00) | 56.97 (55.45) |
| S2004 Ex5 | 72.33 | 71.89 (71.35) | 70.75 (69.70) | 72.73 (72.09) | 73.58 (72.00) | 70.68 (**67.67**) | 70.17 (**67.67**) | 71.92 (69.61) | 72.93 (70.94) | 72.65 (72.25) | 72.05 (70.51) |
| S2005 Ex2 | 19.01 | 19.13 (19.13) | 19.13 (19.13) | 19.43 (19.13) | 19.72 (19.13) | 19.13 (19.13) | 19.43 (19.13) | 19.43 (19.13) | 19.55 (19.13) | 19.13 (19.13) | 19.13 (19.13) |
| S2005 Ex3 | 29.76 | 28.19 (28.00) | 27.91 (27.64) | 28.54 (28.00) | 28.89 (28.36) | 27.68 (27.20) | 27.92 (**27.07**) | 28.20 (27.95) | 29.15 (28.43) | 27.63 (27.13) | 28.08 (28.00) |

## Brecha media respecto de la literatura

| Grupo | base | mult | strk | expl | swap | mix | comp | all | ms | mm |
|---|---|---|---|---|---|---|---|---|---|---|
| Stoyan 2004 (Ex2–5), media | +2.9% | **+0.6%** | +6.0% | +4.4% | +2.2% | +2.0% | +3.7% | +4.5% | +1.7% | +1.5% |
| Stoyan 2004 (Ex2–5), mejor | +0.9% | -0.8% | +3.6% | +2.7% | -0.8% | -0.3% | +0.8% | +1.5% | +0.3% | +0.1% |
| Stoyan 2005 (Ex2–3), media | -2.3% | -2.8% | -1.0% | +0.4% | -3.2% | -2.0% | -1.5% | +0.4% | **-3.3%** | -2.5% |
| Instancias con mejor media que base | | 5 de 7 | 0 | 1 | 4 | 4 | 0 | 1 | 4 | 4 |

## Conclusiones

* **Pesos multiplicativos (`--gls mult`)**: la mejor opción individual (Stoyan 2004: +2.9 % → +0.6 % en media).
  Queda por defecto. Con +1 los pesos crecen igual para todos los pares y dejan de distinguir los solapamientos
  graves; con decaimiento la búsqueda olvida los conflictos ya resueltos.
* **Intercambio de piezas (`--kick swap`)**: ayuda sobre base (encontró 67.67 en S2004 Ex5 y 27.20 en S2005 Ex3),
  pero combinado con mult no mejora la media de mult sola (ms vs mult: 2 mejor, 3 peor). Queda como opción.
* **Strikes, exploración sin subir la altura y compresión** empeoran en casi todas las instancias. Con nuestros
  movimientos exactos por los ejes, la búsqueda necesita muchas pasadas (gls_iter 200) para separar; volver a la
  mejor disposición la corta antes de tiempo. La compresión casi nunca separa con bajadas de 0.05 % (p. ej.,
  0 de 2204 intentos en S2005 Ex2) y le quita el 20 % del tiempo a la oscilación.
* Nuevas mejores soluciones (verificadas con `PLP_check` y `tools/check_solution.py`, eps 1e-9; en
  `solutions/`): S2004 Ex1 52.00, Ex3 **40.24**, Ex4 **53.33**, Ex5 **67.67**; S2005 Ex3 **27.07** (en negrita, menores
  que la mejor publicada).

## Separación más rápida y más pasadas

El perfil (gprof, Stoyan 2004 Example4) mostró que `Placer::nfp` (búsqueda en un `std::map`) tomaba el 40 % del
tiempo y `axis_move` otro 43 %. Cambios (mismo algoritmo, ~5x más rápido en la separación por los ejes):
acceso directo a los NFP; filtro por las cajas de las piezas en `axis_move`; facetas guardadas como `C − K·s`;
cada término se evalúa solo en los candidatos dentro de su intervalo; matriz de penetraciones actualizada por
fila. Con la misma semilla, los primeros pasos llegan a las mismas alturas en 0.6 s en vez de 3 s.

Media (mejor) de 3 semillas, 120 s. `fast`: versión rápida con los parámetros de `mult`; `fls`: búsqueda local
rápida (solo se mueven las piezas activas, Egeblad et al. 2009; Umetani et al. 2009).

| Instancia | Ref. | mult | fast | fls | it500 | it1000 | fls500 |
|---|---|---|---|---|---|---|---|
| S2004 Ex1 | — | 52.52 (52.00) | 52.11 (51.47) | 52.50 (52.30) | 52.28 (52.00) | 51.65 (51.53) | 51.72 (51.37) |
| S2004 Ex2 | 28.47 | 29.00 (29.00) | 29.00 (29.00) | 29.23 (29.00) | 29.00 (29.00) | 29.00 (29.00) | 29.00 (29.00) |
| S2004 Ex3 | 41.30 | 42.12 (41.88) | 40.29 (40.00) | 40.67 (40.04) | 39.17 (**38.93**) | 40.34 (40.06) | 40.58 (40.03) |
| S2004 Ex4 | 54.83 | 55.18 (53.33) | 53.87 (**53.12**) | 54.47 (54.04) | 54.60 (53.39) | 54.01 (53.40) | 56.86 (56.46) |
| S2004 Ex5 | 72.33 | 70.75 (69.70) | 67.24 (**65.50**) | 68.47 (67.48) | 67.75 (66.10) | 67.41 (65.75) | 67.31 (66.09) |
| S2005 Ex2 | 19.01 | 19.13 (19.13) | 19.42 (19.13) | 19.13 (19.13) | 19.13 (19.13) | 19.13 (19.13) | 19.13 (19.13) |
| S2005 Ex3 | 29.76 | 27.91 (27.64) | 28.11 (27.86) | 27.47 (27.18) | 27.59 (27.33) | 27.40 (**26.91**) | 27.53 (27.38) |

| Brecha media | mult | fast | fls | it500 | it1000 | fls500 |
|---|---|---|---|---|---|---|
| Stoyan 2004 (Ex2–5) | +0.6% | -2.3% | -1.2% | **-2.5%** | -2.2% | -0.8% |
| Stoyan 2005 (Ex2–3) | -2.8% | -1.7% | -3.5% | -3.3% | **-3.7%** | -3.4% |
| Instancias con mejor media que mult | | 4 (2 peor) | 5 (1 peor) | **5 (0 peor)** | | |

* La velocidad se traduce en calidad: con más pasadas por separación (`--gls_iter 500`, ahora por defecto) la
  media queda bajo la literatura en S2004 Ex3–5 y S2005 Ex3. 1000 pasadas no mejora más y `--fls` no aporta
  (con el mismo esfuerzo mueve menos piezas por pasada).
* S2004 Ex2 (29.00 vs 28.47) y S2005 Ex2 (19.13 vs 19.01) siguen sin cambios con ninguna configuración.
* Nuevas mejores soluciones (en `solutions/`, verificadas con `PLP_check` y `tools/check_solution.py`, eps 1e-9):
  S2004 Ex1 51.47, Ex3 **38.93**, Ex4 **53.12**, Ex5 **65.50**; S2005 Ex3 **26.91**.
