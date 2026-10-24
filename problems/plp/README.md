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
Esto permite compactar soluciones con programación lineal (SoPlex).

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

Versión por defecto (la que rindió mejor en la ablación, `results/ablacion.md`):

1. **Construcción**: un greedy ubica las piezas una a una (posiciones del `Placer`, evaluación `PLP_Function`).
2. **Compactación** (`Placer::improve` y `Placer::compact`).
3. **Oscilación estratégica** hasta agotar el tiempo: se baja la altura un 1 % (`--sep_step`) y se separan las
   piezas con movimientos de mínimo solapamiento por los ejes; si se logra, se compacta y se vuelve a bajar; si
   no, se sube la altura un cuarto del paso y se sigue desde la disposición solapada; al volver a la altura de
   la mejor solución, kick (se reinserta una pieza al azar de la mejor solución en el espacio libre).

Componentes:

* **Posiciones** (`Placer`): la pieza parte sobre la carga (z = altura actual) en una grilla de
  `grid x grid` puntos (x,y) y desciende por LP. Para cada parte convexa cercana se usa su NFP
  `B ⊕ (−A)` (envolvente convexa de las diferencias de vértices, en caché por par de partes) y se elige
  la faceta que la posición actual satisface y que permite avanzar más en la dirección de descenso; con
  esas facetas el conjunto de posiciones válidas es convexo y se minimiza `z + slide·(x,y hacia la
  pared más cercana)` con un LP de 3 variables (`SmallLP`, conjunto activo desde un punto factible).
  Se repite (región de confianza del tamaño de la pieza) hasta que la posición no mejora.
* **Evaluación** (`PLP_Function`): `alpha·log(V/Vmax) − beta·z/H0 − gamma·max(0, ztop − top)/H0`.
  Valores por defecto ajustados con Optuna (`results/ajuste.md`): `alpha` 1, `beta` 5.53, `gamma` 0.77,
  `slide` 0.065, `grid` 4.
* **Mejora local** (`Placer::improve`): cada pieza (con las demás fijas) vuelve a descender por LP, y las piezas
  que definen la altura se prueban en otras posiciones; se repite mientras la altura baje.
* **Compactación global por LP** (`Placer::compact`, requiere SoPlex; `--no_compact` la desactiva): un LP con las
  traslaciones de todas las piezas y la altura `H` como variables (minimiza `H`). Para cada par de partes que
  pueden tocarse dentro de la región de confianza (cada pieza se mueve a lo más la mitad de la menor dimensión de
  las piezas, por eje) se fija una faceta de su NFP: `n·(t_i − t_j) >= d`. Se resuelve con SoPlex,
  se eligen de nuevo las facetas y se repite mientras `H` baje. Se alterna con la mejora local.
  Sin LP el algoritmo funciona casi igual (~1 % peor en Stoyan 2004).
* **Movimientos de mínimo solapamiento** (`Placer::axis_move`, `Placer::minimize_overlap`; Egeblad et al. 2009,
  Umetani et al. 2009): una pieza se traslada a lo largo de un eje a la posición que minimiza la suma ponderada de
  penetraciones con las demás (con los NFP: `max(0, min_f (d_f − n_f·rel))`, cóncava en un intervalo y cero fuera;
  el mínimo exacto está en los extremos de los intervalos). La búsqueda local mueve las piezas solapadas por los
  tres ejes (hasta `--gls_iter` = 500 pasadas) y en los mínimos locales actualiza los pesos de los pares
  solapados (búsqueda local guiada; por defecto como en sparrow de Gardeyn et al. 2025: el peso de un par
  solapado se multiplica por 1.2–2.0 según la severidad y el de los demás decae ×0.95; `--gls add` suma 1).
  Los pesos persisten durante la oscilación y se borran tras una separación exitosa o un kick.
* Una nueva mejor solución se acepta solo si `Solution::verify` la declara factible.
* **Separación final con margen** (`--margin`, 1e-7; requiere SoPlex): una última compactación por LP con
  `n·(t_i − t_j) ≥ d + m_k` y `x_i + w_i ≤ W − e_x` (lo mismo en y), con holguras `m_k, w_i ∈ [0, margin]` que se
  maximizan sin subir la altura (sube ~1e-7). Deja una separación real, mayor que las tolerancias del LP, entre
  las partes cercanas y respecto de las paredes, para que la solución se pueda certificar en aritmética exacta.
  Luego las coordenadas se redondean a múltiplos de 2^-30 (exacto en doubles): en filas que llenan exactamente
  el contenedor (sin holgura posible) las posiciones de contacto suelen ser sumas de dimensiones y el redondeo
  elimina el error del LP. Con esto, 22 de 22 soluciones de prueba quedaron certificadas (`results/certificacion.md`).

Opciones (desactivadas por defecto; no mejoraron en las instancias evaluadas):

* `--init bsg`: construcción con BSG + `DoubleEffort` durante `--bsg_time` (30 %) del tiempo.
* `--blocks N`: pseudobloques (pares de piezas en una posición relativa compacta que el BSG o el greedy ubican
  como una sola forma; `Placer::generate_blocks`, `results/pseudobloques.md`). Se aceptan si el hueco agregado
  (vol. de la envolvente convexa del bloque − vol. de las envolventes de las piezas) / vol. de las piezas
  ≤ `--block_waste` (0.07).
* `--sep_lp`: si los movimientos por los ejes fallan, separación por LP (`Placer::separate`): variables `t_i` y
  una holgura por par de partes cercanas, `n·(t_i − t_j) + s ≥ d` para una faceta del NFP, minimiza `Σ w_ij s`
  (Gomes y Oliveira 2006; Egeblad et al. 2009).
* `--repair`: tras `--kicks` (30) kicks sin mejora, el BSG resuelve la variante knapsack con altura
  `H·(1 − step)`, manteniendo las piezas de la mejor solución bajo `(1 − band)·H` (destruir y reparar).
* `--kick swap`: el kick intercambia dos piezas de distinto tipo y volumen parecido.
* Evaluadas y eliminadas (`results/papers.md`): volver a la mejor disposición solapada (strikes), exploración sin
  subir la altura, compresión final, búsqueda local rápida y kicks de varias piezas.

### Compilación con SoPlex (opcional)

SoPlex (https://github.com/scipopt/soplex, licencia Apache 2.0), solo la biblioteca:

```
git clone --depth 1 -b v8.1.0 https://github.com/scipopt/soplex.git
cmake -S soplex -B soplex/build -DCMAKE_BUILD_TYPE=Release -DZLIB=off -DGMP=off -DBOOST=off -DMPFR=off -DPAPILO=off -DCMAKE_INSTALL_PREFIX=$HOME/soplex
cmake --build soplex/build -j4 && cmake --install soplex/build
```

y luego, en Metasolver: `cmake .. -DSOPLEX_DIR=$HOME/soplex`. Sin SoPlex todo compila igual, pero sin la
compactación global ni la separación final con margen. `repro/setup.sh` hace todo esto automáticamente.

```
./BSG_PLP "problems/plp/benchs/Stoyan et al. 2005/Example1.json" -t 30 -o sol.json
```

### Resultados

Versiones anteriores (con BSG; histórico). Efecto de la compactación global (30 s por instancia, 4 procesos en
paralelo; ibex/SoPlex):

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

Con la bajada de altura y contenedor fijo (30 s, ibex/SoPlex; tabla completa en
`results/bsg_plp_v2.md`): Stoyan 2005 Example1–3: 17.84 / 20.30 / 34.15; Stoyan 2004 Example1–5:
60.56 / 36.00 / 58.38 / 73.47 / 90.68.

Comparación con la literatura (120 s, media de 3 semillas y mejor corrida, versión por defecto actual;
`results/ablacion.md`). La última columna es la mejor solución encontrada con cualquier configuración evaluada;
esas soluciones están en `results/solutions/`, verificadas con `PLP_check` y `tools/check_solution.py` (eps 1e-9):

| Instancia | Ítems | Mejor publicada | BSG_PLP (120 s): media (mejor) | Mejor nuestra |
|---|---|---|---|---|
| Stoyan 2005 Example1 | 7 | 17.46 (óptimo, Tollenaere et al. 2026) | 17.46 (30 s) | 17.46 |
| Stoyan 2005 Example2 | 12 | 19.01 (Egeblad et al. 2009) | 19.13 (19.13) | 19.13 |
| Stoyan 2005 Example3 | 25 | 29.76 (Lamas-Fernández et al. 2023) | **27.52 (27.00)** | **26.91** |
| Stoyan 2004 Example1 | 25 | — | 53.21 (52.75) | 51.47 |
| Stoyan 2004 Example2 | 20 | 28.47 (Lamas-Fernández et al. 2023) | 29.00 (29.00) | 29.00 |
| Stoyan 2004 Example3 | 30 | 41.30 (Lamas-Fernández et al. 2023) | **40.15 (40.00)** | **38.93** |
| Stoyan 2004 Example4 | 40 | 54.83 (Lamas-Fernández et al. 2023) | **52.69 (50.91)** | **50.91** |
| Stoyan 2004 Example5 | 50 | 72.33 (Lamas-Fernández et al. 2023) | **67.21 (66.54)** | **64.20** |

Ablación de los componentes (la versión mínima rinde igual): `results/ablacion.md`.

Instancias Merged de Egeblad et al. (2009), 600 s (`results/merged.md`; piezas de Ikonen descompuestas de forma
exacta con `tools/decompose_prism.py`):

| Instancia | Ítems | 3DNest (Egeblad et al. 2009) | Lamas-Fernández et al., 4 h | BSG_PLP (versión por defecto actual): media (mejor) |
|---|---|---|---|---|
| Merged1 | 15 | 17.00 | 17.13 | **16.60 (16.55)** |
| Merged2 | 30 | 21.98 | 22.06 | **21.00 (21.00)** |
| Merged3 | 45 | 24.04 | 24.32 | **22.07 (22.00)** |
| Merged4 | 60 | 27.81 | 26.47 | **24.47 (24.01)** |
| Merged5 | 75 | 30.13 | 29.24 | **27.04 (27.00)**; completa: 26.40 |

**Advertencia sobre las referencias de Stoyan 2004**: Lamas-Fernández et al. (2023) voxelizan las piezas
sobreestimándolas (en Merged1, +15 % de volumen) y las demás referencias con geometría exacta son más débiles
(Stoyan et al. 2004, HAPE3D); la comparación no es entre iguales. Stoyan 2005 Example3 y las Merged sí son la
misma instancia exacta que 3DNest (Egeblad et al. 2009).

Lamas-Fernández et al. (2023) usan piezas voxelizadas (algo más grandes) y reportan el mejor de 10 corridas de 1 h.

Resultados iniciales sin ibex: `results/bsg_plp_v1.md`.

## Herramientas

`tools/certify_solution.py instancia.json solucion.json benchs` (Python, numpy y scipy): **certificado exacto**,
sin tolerancias. Los vértices y las traslaciones son doubles, es decir, racionales exactos; con `fractions` se
verifica la contención y, para cada par de partes cercanas, un eje separador (normales de las caras y productos
cruz de aristas, en racionales) con `max_A n·a <= min_B n·b`. Si falla, informa la violación exacta. Las piezas que
se tocan exactamente (p. ej., una fila que llena el ancho del contenedor) pueden no tener posiciones de contacto
representables en doubles; la separación final con margen lo evita cuando hay holgura.

`tools/decompose_prism.py benchs pieza.stl ...`: descomposición convexa exacta de piezas prismáticas (extrusión de
un polígono); escribe `benchs/decompositions/<pieza>.obj` y verifica que el volumen de las partes sea el de la malla.

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
* `LPBackend`: interfaz para LPs grandes; implementación con SoPlex si se compila con `SOPLEX_DIR`.
