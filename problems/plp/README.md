# PLP: carga de poliedros (3D irregular strip packing)

## Definición del problema

Variante *3D irregular open dimension problem* (I3DODP), también llamada
*3D irregular strip packing*, en la forma usada por Stoyan et al. (2004, 2005),
Egeblad et al. (2009), Lamas-Fernández et al. (2022) y Tollenaere et al. (2025):

* **Contenedor**: paralelepípedo de base fija `W x D` (ejes x e y) y altura `H` abierta (eje z).
* **Piezas**: tipos de pieza `i` con demanda `d_i`. Cada pieza es un poliedro, representado
  como una **unión de politopos convexos** `P_i = C_i1 ∪ ... ∪ C_ik`.
* **Decisión**: una traslación `t ∈ R³` por cada pieza. **No se permiten rotaciones**
  (la orientación de la pieza es la del archivo).
* **Factibilidad**: cada pieza queda dentro de `[0,W] x [0,D] x [0,H]` y los interiores de dos
  piezas distintas no se intersectan (pueden tocarse).
* **Objetivo**: ubicar todas las piezas minimizando `H`.

Sin rotaciones, la no-superposición entre dos partes convexas `C` y `C'` con un hiperplano
separador de normal `a` fija es lineal en las traslaciones `t`, `t'` y el término `b`:
`a·(v + t) <= b` para los vértices `v` de `C` y `a·(w + t') >= b` para los vértices `w` de `C'`.
Esto permite compactar soluciones con programación lineal (p.ej., SoPlex/CLP vía ibex).

Extensiones previstas: rotaciones ortogonales (24 orientaciones, cada una se trata como otra
pieza) y la variante knapsack (altura fija, maximizar el volumen cargado).

## Instancias (`benchs/`)

Copia de [ESICUP/datasets › 3d_irregular](https://github.com/ESICUP/datasets/tree/main/3d_irregular)
(licencia CC0, commit `154a8f0`), sin los conjuntos de maximización de volumen (Silva et al. 2021,
Tollenaere et al. 2024) ni los modelos voxelizados de Lamas-Fernández et al. (2022).
Ver `benchs/README_ESICUP.md` para las referencias y el formato JSON.

| Conjunto | Instancias | Piezas | Partes convexas |
|---|---|---|---|
| `Tollenaere, J. et al. 2025/Instances/STOYAN_2005` | 2–20 ítems | convexas | sí |
| `Tollenaere, J. et al. 2025/Instances/TETRAHEDRONS` | 2–20 ítems | tetraedros | sí |
| `Tollenaere, J. et al. 2025/Instances/DIAMONDS` | 2–10 ítems | convexas (86–308 caras) | sí |
| `Tollenaere, J. et al. 2025/Instances/STOYAN_2004` | 2–20 ítems | convexas | sí (1) |
| `Stoyan et al. 2005` Example1–3 | 7, 12, 25 ítems | convexas | sí |
| `Stoyan et al. 2004` Example1 | 80 ítems | convexas | sí (1) |
| `Stoyan et al. 2004` Example2–5 | 20–50 ítems | no convexas | sí (2) |
| `Liu et al. 2015` Experiment1 | 36 ítems | 3 de 5 no convexas | **no** |
| `Egeblad et al. 2009` Merged1–5 | 15–75 ítems | 6 de 15 no convexas | **no** |

1. `convex/polytope1.obj` tiene una cara corrupta; se usa la envolvente convexa de sus vértices
   (`benchs/decompositions/Stoyan et al. 2004/convex/polytope1.obj`).
   Del mismo modo, las caras de `Stoyan et al. 2005/polytope7.obj` usan solo 9 de sus 10 vértices (falta
   (−2,−5,2)); según la tabla 1 de Stoyan et al. (2005) P7 tiene 10 vértices, así que se usa la envolvente de
   los 10 (volumen 192.5, igual a `Convex7.stl` de Egeblad et al. 2009):
   `benchs/decompositions/Stoyan et al. 2005/polytope7.obj`. Afecta a Stoyan 2005 Example1–3 y a las
   instancias STOYAN_2005 de Tollenaere; los resultados anteriores a esta corrección en esas instancias usaban
   una pieza más pequeña y no son válidos.
2. Las instancias usan las mallas `nonconvex/sanitized`; sus partes convexas se leen de
   `benchs/decompositions/...`, que son copias de `nonconvex/polytopeK.obj` (la unión de
   convexos descrita por Stoyan et al.). Algunas partes se solapan; el volumen de la pieza se
   toma de la malla sanitizada.

**Descomposiciones**: si existe `benchs/decompositions/<ruta de la pieza con extensión .obj>`,
las partes convexas se leen de ese archivo (una parte por grupo `o`/`g`, o por cascarón cerrado).
Las piezas no convexas de Liu et al. (2015) y Egeblad et al. (2009) todavía no tienen descomposición.

Mejor solución conocida incluida: `Stoyan et al. 2005/Solution1.json` (Example1, H = 17.463).

## Literatura y resultados por recolectar

Para comparar hay que extraer, por instancia, la altura obtenida (y la mejor), el tiempo y la máquina.

| # | Referencia | Qué extraer |
|---|---|---|
| 1 | Stoyan, Gil, Scheithauer, Pankratov, Magdalina (2005). *Packing of convex polytopes into a parallelepiped*. Optimization 54(2):215–235. doi:10.1080/02331930500050681 | Alturas de Example1–3 (convexas) |
| 2 | Stoyan, Gil, Pankratov, Scheithauer (2004). *Packing non-convex polytopes into a parallelepiped*. Preprint MATH-NM-06-2004, TU Dresden | Alturas de Example1–5 |
| 3 | Egeblad, Nielsen, Brazil (2009). *Translational packing of arbitrary polytopes*. Computational Geometry 42(4):269–288. doi:10.1016/j.comgeo.2008.06.003 | Merged1–5 y Stoyan 2005 (solo traslaciones, igual que aquí) |
| 4 | Liu, Liu, Cao, Yao (2015). *HAPE3D — a new constructive algorithm for the 3D irregular packing problem*. Frontiers of Information Technology & Electronic Engineering 16(5):380–390. doi:10.1631/FITEE.1400421 | Experiment1 y Stoyan 2004 (Experiment2–5); revisar si permite rotaciones |
| 5 | Lamas-Fernández, Bennell, Martínez-Sykora (2023). *Voxel-based solution approaches to the three-dimensional irregular packing problem*. Operations Research 71(4):1298–1317. doi:10.1287/opre.2022.2260 | Tablas del artículo y del e-companion: mejores alturas conocidas de casi todos los conjuntos (p.ej., Merged5 = 29.24); revisar si permite rotaciones |
| 6 | Tollenaere, Martínez-Sykora, Wauters (2026). *Mixed-integer linear programming models for 3D irregular strip packing problems*. EJOR 331(2):365–380. doi:10.1016/j.ejor.2025.10.041 | Óptimos y cotas de STOYAN_2004, STOYAN_2005, TETRAHEDRONS y DIAMONDS (2–20 ítems) |
| 7 | Romanova, Bennell, Stoyan, Pankratov (2018). *Packing of concave polyhedra with continuous rotations using nonlinear optimisation*. EJOR 268(1):37–53. doi:10.1016/j.ejor.2018.01.025 | Con rotaciones continuas: resultados en instancias previas (referencia, no comparable directamente) |
| 8 | Egeblad, Nielsen, Odgaard (2007). *Fast neighborhood search for two- and three-dimensional nesting problems*. EJOR 183(3):1249–1266 | Resultados 3D previos de la misma línea que [3] |
| 9 | Ma, Chen, Hu, Wang (2018). *Packing irregular objects in 3D space via hybrid optimization*. Computer Graphics Forum 37(5) (SGP 2018) | Uniones de convexos y comparación con instancias de la literatura |
| 10 | Leão, Toledo, Oliveira, Carravilla, Alvarez-Valdés (2020). *Irregular packing problems: A review of mathematical models*. EJOR 282(3):803–822 | Revisión: modelos y referencias adicionales |
| 11 | Tollenaere, Çalık, Wauters (2024). *Efficient use of collision detection for volume maximization problems*. EJOR 319(3):967–982. doi:10.1016/j.ejor.2024.05.048 | Técnicas de detección de colisiones (no tiene instancias de carga) |

Metodología (2D) en la que se basa la compactación por LP:
* Li, Milenkovic (1995). *Compaction and separation algorithms for non-convex polygons and their applications*. EJOR 84(3):539–561.
* Gomes, Oliveira (2006). *Solving irregular strip packing problems by hybridising simulated annealing and linear programming*. EJOR 171(3):811–829.

## Algoritmo (BSG_PLP)

* **Estado** (`plpState`): piezas ubicadas y restantes. Una acción ubica una pieza en una posición factible.
  El valor de una solución completa es la densidad `V / (W·D·H)`.
* **Posiciones** (`Placer`): la pieza parte sobre la carga (z = altura actual) en una grilla de
  `grid x grid` puntos (x,y) y desciende por LP. Para cada parte convexa cercana se usa su NFP
  `B ⊕ (−A)` (envolvente convexa de las diferencias de vértices, en caché por par de partes) y se elige
  la faceta que la posición actual satisface y que permite avanzar más en la dirección de descenso; con
  esas facetas el conjunto de posiciones válidas es convexo y se minimiza `z + slide·(x,y hacia la
  pared más cercana)` con un LP de 3 variables (`SmallLP`, conjunto activo desde un punto factible).
  Se repite (región de confianza del tamaño de la pieza) hasta que la posición no mejora.
* **Evaluación** (`PLP_Function`): `alpha·log(V/Vmax) − beta·z/H0 − gamma·max(0, ztop − top)/H0`.
  Valores por defecto ajustados con Optuna (`results/ajuste.md`): `alpha` 1, `beta` 5.53, `gamma` 0.77,
  `slide` 0.065, `grid` 4, `block_waste` 0.07.
* **Pseudobloques** (`--blocks N`, `Placer::generate_blocks`): pares de piezas en una posición relativa
  compacta, que el BSG puede ubicar como una sola forma (como los bloques del CLP). Un bloque es la unión de
  las partes convexas de sus dos piezas (no es convexo); para el `Placer` es una forma más, con partes
  desplazadas, que reutiliza los NFP ya calculados.
  * Posiciones relativas candidatas: vértices de los NFP entre las partes y, para cada faceta del NFP, los
    puntos de su plano sobre los ejes (deslizar una pieza junto a la otra a lo largo de un eje). Se evalúan
    las 30 factibles de menor caja envolvente.
  * Criterio de aceptación: hueco agregado = (vol. de la envolvente convexa del bloque − vol. de las
    envolventes de las dos piezas) / volumen de las piezas ≤ `--block_waste` (0.1). En piezas convexas es el
    espacio atrapado entre ellas; es negativo si una pieza entra en una concavidad de la otra.
  * Se conservan los `--block_pair` (2) de menor hueco por par de tipos y los N de menor hueco en total.
  * Por defecto se generan 30 bloques (`--blocks 0` los desactiva). `--block_criterion box` usa el criterio
    anterior (caja del bloque al menos 5% menor que la suma de las cajas) y `--block_eval piece` evalúa un
    bloque con el volumen por pieza en vez del total. Con 3 semillas, la combinación por defecto
    (`hull` + `total`) fue la mejor en conjunto (`results/pseudobloques.md`).
* **Búsqueda**: BSG con `DoubleEffort` (como en el CLP).
* **Mejora final** (`Placer::improve`): cada pieza (con las demás fijas) vuelve a descender por LP, y
  las piezas que definen la altura se prueban en otras posiciones; se repite mientras la altura baje.
* **Compactación global por LP** (`Placer::compact`, requiere ibex): un LP con las traslaciones de
  todas las piezas y la altura `H` como variables (minimiza `H`). Para cada par de partes que pueden
  tocarse dentro de la región de confianza (cada pieza se mueve a lo más la mitad de la menor dimensión
  de las piezas, por eje) se fija una faceta de su NFP: `n·(t_i − t_j) >= d`. Se resuelve con
  `ibex::LPSolver` (SoPlex o CLP), se eligen de nuevo las facetas y se repite mientras `H` baje.
  En `BSG_PLP` se alterna con la mejora local sobre la mejor solución; con `--compact_greedy k`
  además se compacta (k iteraciones) cada solución completa del greedy dentro del BSG.
* **Bajar la altura y resolver con un contenedor fijo** (por defecto, después de `--strip_time` = 30% del
  tiempo): se fija `Hc = H·(1 − step)` y se resuelve la variante knapsack (contenedor `W x D x Hc`, valor =
  volumen cargado / volumen total; las posiciones candidatas que sobrepasan `Hc` se descartan). El BSG parte
  con las piezas de la mejor solución que quedan bajo `(1 − band)·Hc` ya ubicadas y decide dónde van las
  demás (destruir y reparar). El BSG se detiene en cuanto ubica todas las piezas; la solución se mejora
  (compactación) y se vuelve a bajar la altura. Si falla, la franja libre crece (`band·1.5`) hasta partir de
  cero (`band = 1`), y luego se baja menos la altura (`step/2`). Cada altura tiene a lo más
  `--slice` = 10% del tiempo. Si la carga parcial no está completa, se completa sobre la carga (altura
  abierta) por si mejora la altura.
* **Separación por LP** (`Placer::separate`, requiere ibex; fase previa a la reparación con BSG): se baja la
  altura un 1% (`--sep_step`), las piezas que sobresalen se bajan aunque queden solapadas y un LP elimina el
  solapamiento: variables `t_i` y una holgura `s ≥ 0` por par de partes cercanas, `n·(t_i − t_j) + s ≥ d` para
  una faceta del NFP (la de mayor holgura; si el par sigue solapado se rota entre las 3 primeras); minimiza
  `Σ w_ij s` con pesos de búsqueda local guiada (aumentan para los pares que siguen solapados). Si se estanca,
  la pieza más pequeña de un par solapado se reinserta en el espacio libre. Si se elimina el solapamiento, se
  mejora la solución y se vuelve a bajar; si no, el paso se reduce a la mitad (hasta 0.1%) y luego se pasa a la
  reparación con BSG (`--no_separate` la desactiva). Basado en Egeblad et al. (2009), Gomes y Oliveira (2006) y
  Lamas-Fernández et al. (2023).
* **Movimientos de mínimo solapamiento** (`Placer::axis_move`, `Placer::minimize_overlap`; Egeblad et al. 2009,
  Umetani et al. 2009): una pieza se traslada a lo largo de un eje a la posición que minimiza la suma ponderada de
  penetraciones con las demás (con los NFP: `max(0, min_f (d_f − n_f·rel))`, cóncava en un intervalo y cero fuera;
  el mínimo exacto está en los extremos de los intervalos). La búsqueda local mueve las piezas solapadas por los
  tres ejes y en los mínimos locales aumenta los pesos de los pares solapados (búsqueda local guiada). Con
  `--sep_method both` (por defecto) se usa antes del LP de separación; `axis` o `lp` usan solo uno.
* **Oscilación estratégica y kicks** (fase de separación): si la separación no elimina el solapamiento, se sube
  la altura un cuarto del paso y se sigue desde la solución solapada (los pesos de la búsqueda local guiada
  persisten). Cuando la altura vuelve a la de la mejor solución, kick: desde la mejor solución se reinserta una
  pieza al azar en el espacio libre (o se intercambian dos piezas de distinto tipo); con `--kick_size k` se
  reinsertan entre 1 y k piezas. Tras `--kicks` (30) kicks
  seguidos sin mejora se intenta la reparación con BSG y luego se vuelve a la oscilación, hasta agotar el tiempo.
* Todas las soluciones se verifican con `Solution::verify`.

### Compilación con ibex (opcional)

ibex con SoPlex (rama `dfb-sc-paper` de https://github.com/rilianx/ibex-lib; necesita `flex` y `bison`):

```
git clone -b dfb-sc-paper https://github.com/rilianx/ibex-lib.git
mkdir ibex-lib/build && cd ibex-lib/build
cmake .. -DLP_LIB=soplex -DCMAKE_INSTALL_PREFIX=$HOME/ibex
make -j4 ibex && make install
```

y luego, en Metasolver: `cmake .. -DIBEX_DIR=$HOME/ibex`. Sin `IBEX_DIR` todo compila igual, pero sin la
compactación global.

```
./BSG_PLP "problems/plp/benchs/Stoyan et al. 2005/Example1.json" -t 30 -o sol.json
```

Efecto de la compactación global (30 s por instancia, 4 procesos en paralelo; ibex/SoPlex):

| Instancia | sin compactación | compactación final | `--compact_greedy 5` |
|---|---|---|---|
| Stoyan 2005 Example1 | 17.97 | 17.84 | 17.84 |
| Stoyan 2005 Example2 | 20.68 | 20.45 | 20.30 |
| Stoyan 2005 Example3 | 35.42 | 35.13 | 35.63 |
| Stoyan 2004 Example1 | 73.30 | 72.32 | 65.81 |
| Stoyan 2004 Example2 | 38.60 | 38.60 | 39.27 |
| Stoyan 2004 Example3 | 59.89 | 57.88 | 57.83 |
| Stoyan 2004 Example4 | 75.07 | 73.59 | 72.75 |
| Stoyan 2004 Example5 | 94.12 | 93.04 | 93.23 |

Con la bajada de altura y contenedor fijo (30 s, parámetros por defecto, ibex/SoPlex; tabla completa en
`results/bsg_plp_v2.md`): Stoyan 2005 Example1–3: 17.84 / 20.30 / 34.15; Stoyan 2004 Example1–5:
60.56 / 36.00 / 58.38 / 73.47 / 90.68.

Comparación con la literatura (120 s, media de 3 semillas y mejor corrida, versión con movimientos de mínimo
solapamiento; `results/largo.md`; soluciones en `results/solutions/`, verificadas con `tools/check_solution.py`):

| Instancia | Ítems | Mejor publicada | BSG_PLP (120 s): media (mejor) |
|---|---|---|---|
| Stoyan 2005 Example1 | 7 | 17.46 (óptimo, Tollenaere et al. 2026) | 17.46 (30 s) |
| Stoyan 2005 Example2 | 12 | 19.01 (Egeblad et al. 2009) | 19.13 (19.13) |
| Stoyan 2005 Example3 | 25 | 29.76 (Lamas-Fernández et al. 2023) | **28.34 (28.00)** |
| Stoyan 2004 Example2 | 20 | 28.47 (Lamas-Fernández et al. 2023) | 29.88 (29.00) |
| Stoyan 2004 Example3 | 30 | 41.30 (Lamas-Fernández et al. 2023) | 42.09 (**41.00**) |
| Stoyan 2004 Example4 | 40 | 54.83 (Lamas-Fernández et al. 2023) | 56.52 (55.55) |
| Stoyan 2004 Example5 | 50 | 72.33 (Lamas-Fernández et al. 2023) | 72.68 (**69.98**) |

Lamas-Fernández et al. (2023) usan piezas voxelizadas (algo más grandes) y reportan el mejor de 10 corridas de 1 h.

Resultados iniciales sin ibex: `results/bsg_plp_v1.md`.

## Herramientas

`tools/check_solution.py instancia.json solucion.json benchs [eps]` (Python, numpy y scipy): verificación
independiente para piezas convexas o uniones de convexos (partes leídas de `benchs/decompositions` o envolvente
de todos los vértices del OBJ; un LP por par de partes para el punto más profundo de la intersección). Detectó el error de `polytope7`, que `PLP_check` no podía ver porque
compartía la lectura de las mallas.

```
PLP_check <instance.json> [-s solution.json] [--eps 1e-6] [--partial] [-q] [--root dir]
```

Muestra la instancia (piezas, volúmenes, cota inferior `max(V/(W·D), altura de la pieza más alta)`)
y, con `-s`, verifica una solución: piezas dentro del contenedor, sin solapamientos (profundidad de
penetración mayor que `eps`, calculada con ejes separadores entre las partes convexas) y con todas
las piezas ubicadas. Retorna 0 si la solución es factible y 2 si no lo es.

Formato de solución (ESICUP): por pieza, el nombre del archivo y una matriz 4x4 en convención de
vectores fila (traslación en la última fila), aplicada a la pieza trasladada para que su caja
envolvente parta en el origen (`"itemOrigin": "AlignToMinimum"`).

```
./PLP_check "problems/plp/benchs/Stoyan et al. 2005/Example1.json" \
            -s "problems/plp/benchs/Stoyan et al. 2005/Solution1.json"
```

## Código

* `Geometry.h`: vectores, matrices y cajas.
* `ConvexPolytope`: envolvente convexa (incremental), representaciones V y H, y profundidad de
  penetración por ejes separadores (normales de facetas y productos cruz de aristas).
* `Mesh`: lectura de OBJ y STL (ASCII y binario).
* `Instance`: piezas como unión de convexos e instancias en formato JSON de ESICUP.
* `Solution`: lectura/escritura de soluciones y verificación.
* `Json`: lector/escritor JSON mínimo (sin dependencias).
* `SmallLP`: LP en 3 variables (conjunto activo, todos los iterados son factibles).
* `Placer`: NFPs, descenso por LP, posiciones candidatas y mejora de soluciones.
* `plpState`: estado, acción y evaluador para el BSG, y `CompactGreedy`; `main_plp.cpp`: `BSG_PLP`.
* `LPBackend`: interfaz para LPs grandes; implementación con `ibex::LPSolver` si se compila con `IBEX_DIR`.
