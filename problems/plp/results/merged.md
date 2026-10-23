# Instancias Merged (Egeblad et al. 2009): comparación con 3DNest y con Lamas-Fernández et al.

## Instancias

Merged*i* tiene *i* copias de cada uno de los 15 poliedros: los 7 convexos de Stoyan et al. (2005) y los 8 de
Ikonen et al. (escalados ×4); el contenedor es tal que una solución con 50 % de utilización es un cubo
(Egeblad et al. 2009, §6.3). Los archivos ESICUP (`benchs/Egeblad et al. 2009/`) coinciden: volúmenes de la
Tabla 1 de Egeblad ×64 para las piezas de Ikonen, Convex7 = 192.5, y Merged1 con base 14.9995 = (2·1687.33)^(1/3).

Las 6 piezas no convexas de Ikonen son prismas (extrusiones de un polígono); se descomponen de forma exacta con
`tools/decompose_prism.py` (polígono de la base → ear clipping → Hertel–Mehlhorn → extrusión; un marco en 4
cajas; Part2 tiene dos polígonos que se tocan en un vértice y Part4 un hueco que toca el borde). El volumen de
las partes coincide con el de la malla (diferencias ≤ 4e-6 por los flotantes de 32 bits del STL).

## Referencias

* 3DNest (Egeblad et al. 2009): geometría exacta, 10 corridas de 600 s, un núcleo de un Core 2 de 2.16 GHz.
  Utilización media a los 10/60/300/600 s y máxima (Tabla 3); la altura se obtiene de la utilización.
* Lamas-Fernández et al.: vóxeles que sobreestiman las piezas (en Merged1 el volumen sube 15 %); VNS
  extra-fino (250 vóxeles en el lado de la base), 4 h (Tabla 11). Mejores conocidas en Merged4 y Merged5.

## Experimento

BSG_PLP, 600 s, semillas 1–3, 4 corridas en paralelo en un Xeon de 2.8 GHz. `mínima`: `--sep_method axis
--blocks 0 --kicks 1000000 --strip_time 0.01` (greedy inicial, separación por los ejes, oscilación con kicks,
compactación por LP); `completa`: parámetros por defecto (BSG inicial durante 180 s). La utilización a los
10/60/300 s se obtiene de las mejoras registradas en el log (con su tiempo).

### Utilización media en el tiempo (nuestra / 3DNest)

`mínima`:

| Instancia | Piezas | 10 s | 60 s | 300 s | 600 s |
|---|---|---|---|---|---|
| Merged1 | 15 | 43.2% / 37.3% | 45.0% / 39.7% | 45.1% / 42.4% | 45.2% / 43.0% |
| Merged2 | 30 | 43.3% / 30.2% | 45.0% / 39.8% | 45.0% / 42.8% | 45.0% / 42.9% |
| Merged3 | 45 | 44.2% / 27.0% | 47.5% / 35.5% | 48.4% / 42.2% | 49.0% / 43.3% |
| Merged4 | 60 | 43.2% / 26.1% | 47.7% / 31.6% | 48.7% / 40.1% | 48.7% / 42.0% |
| Merged5 | 75 | 43.2% / 26.7% | 45.7% / 29.6% | 47.2% / 38.0% | 47.4% / 41.0% |

`completa` (sin solución antes de 180 s): 300 s: 44.8 / 45.0 / 48.1 / 47.1 / 47.2 %; 600 s: 45.1 / 45.0 /
48.4 / 48.8 / 47.6 %.

### Alturas

| Instancia | 3DNest mejor (util.) | Lamas 4 h (util.) | mínima: media (mejor) | completa: media (mejor) | Mejor nuestra (util.) |
|---|---|---|---|---|---|
| Merged1 | 17.00 (44.1%) | 17.13 (43.8%) | 16.60 (16.55) | 16.64 (16.55) | **16.55** (45.3%) |
| Merged2 | 21.98 (43.0%) | 22.06 (42.8%) | 21.00 (21.00) | 21.02 (21.00) | **21.00** (45.0%) |
| Merged3 | 24.04 (45.0%) | 24.32 (44.5%) | 22.07 (22.00) | 22.33 (22.00) | **22.00** (49.2%) |
| Merged4 | 27.81 (42.8%) | 26.47 (45.0%) | 24.47 (24.01) | 24.41 (24.04) | **24.01** (49.6%) |
| Merged5 | 30.13 (42.6%) | 29.24 (43.9%) | 27.04 (27.00) | 26.97 (26.40) | **26.40** (48.6%) |

Las mejores soluciones están en `solutions/` (verificadas con `PLP_check` y `tools/check_solution.py`, eps 1e-9).

## Conclusiones

* A los 10 s la versión mínima supera la utilización media de 3DNest a los 600 s en todas las instancias; aun
  suponiendo que esta máquina sea 4–5 veces más rápida por núcleo, la ventaja se mantiene a igual tiempo efectivo.
* La ventaja crece con el tamaño: en Merged4–5 la mejor nuestra está ~9–10 % bajo la mejor conocida (Lamas,
  4 h). Las instancias combinan piezas no convexas y convexas: la geometría exacta (sin la sobreestimación de los
  vóxeles), la separación por los ejes con los NFP y la compactación por LP pesan más que en las instancias
  pequeñas.
* La versión completa llega a valores finales parecidos, pero el BSG inicial no deja una solución antes de 180 s.
* Error encontrado en este experimento (corregido): en una corrida, el LP de compactación devolvió una pieza
  fuera del contenedor y se aceptó como mejor solución; ahora `compact()` revisa los límites y solo se aceptan
  soluciones verificadas. Esa corrida se repitió (`completa`, semilla 1, Merged4: 25.08).
