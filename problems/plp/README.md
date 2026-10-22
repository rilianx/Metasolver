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

Comparación con la literatura (detalle en `results/comparacion.md`):

| Instancia | Ítems | Mejor conocida | BSG_PLP (30 s) |
|---|---|---|---|
| Stoyan 2005 Example1 | 7 | 17.46 (óptimo, Tollenaere et al. 2026) | 17.84 (+2.2%) |
| Stoyan 2005 Example2 | 12 | 19.01 (Egeblad et al. 2009) | 20.30 (+6.8%) |
| Stoyan 2005 Example3 | 25 | 29.76 (Lamas-Fernández et al. 2023) | 34.15 (+14.8%) |
| Stoyan 2004 Example2–5 | 20–50 | 28.47 / 41.30 / 54.83 / 72.33 (Lamas-Fernández et al. 2023) | 36.00 / 58.38 / 73.47 / 90.68 (+25% a +41%) |
| Tollenaere 2025 (66 instancias) | 2–20 | MILP, 1 h | 10 s: mejor en 10, igual en 20, peor en 36 |

Resultados iniciales sin ibex: `results/bsg_plp_v1.md`.

## Herramientas

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
