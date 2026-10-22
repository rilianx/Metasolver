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
