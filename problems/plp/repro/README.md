# Reproducción de los experimentos del paper

Scripts para correr todos los experimentos del paper (`problems/plp/paper`) en otra máquina y regenerar sus tablas.

## Pasos

```bash
# 0. dependencias del sistema (Debian/Ubuntu)
sudo apt install build-essential cmake git libqhull-dev python3-venv

# 1. SoPlex + BSG_PLP + entorno Python + instancia Chess y sus descomposiciones + prueba rápida (~15 min)
JOBS=14 problems/plp/repro/setup.sh

# 2. (opcional) prueba de todo el flujo con los tiempos divididos por 60 (~10 min)
JOBS=14 TDIV=60 OUT=$PWD/problems/plp/repro/out/test/runs problems/plp/repro/run_experiments.sh
OUT=$PWD/problems/plp/repro/out/test/runs problems/plp/repro/analyze.sh

# 3. experimentos completos (~3 h con 14 procesos; conviene dentro de tmux/screen o con nohup)
JOBS=14 nohup problems/plp/repro/run_experiments.sh > problems/plp/repro/out/run.log 2>&1 &

# 4. certificación exacta y tablas -> problems/plp/repro/out/results.md
JOBS=14 problems/plp/repro/analyze.sh            # con --paper: copia las trayectorias y regenera las figuras
```

`run_experiments.sh` acepta grupos (`chess main ablation kicks sens tollenaere liu`) y omite las corridas ya terminadas, así
que puede relanzarse si se interrumpe.

| Grupo | Corridas | Tiempo por corrida | Tiempo de CPU |
|---|---|---|---|
| main: Stoyan 2004/2005, Merged1–5 | 13 × 10 semillas | 120 s | 4.3 h |
| ablation: 7 instancias × (sin LP, pesos aditivos, solo reinserción) | 210 | 120 s | 7 h |
| kicks: 7 instancias × (ninguno, mixto, pieza superior, pieza superior con menor penetración) | 280 | 120 s | 9.3 h |
| sens: 7 instancias × (ρ = 0.5 %, 2 %; 100 y 2000 pasadas) | 280 | 120 s | 9.3 h |
| tollenaere: Tollenaere et al. 2025 | 66 × 10 | 30 s | 5.5 h |
| liu: Liu et al. 2015 Experiment1 | 10 | 120 s | 0.3 h |
| chess: fina, gruesa, gruesa + fina | 3 × 5 semillas | 1200 s | 5 h |

Total ≈ 40 h de CPU, ≈ 3 h con 14 procesos.

## Notas

* Cada corrida usa un hilo y el tiempo límite es de CPU (`clock()`). Para que los tiempos sean comparables conviene
  no superar el número de núcleos **físicos** (el script avisa si `JOBS` lo supera) y no correr otras cargas a la vez.
  Si la máquina tiene 16 núcleos, 14 procesos dejan dos libres para el sistema.
* `hardware.sh` guarda en `out/hardware.txt` la CPU (`lscpu`), núcleos físicos y lógicos, frecuencias, memoria,
  sistema operativo, compiladores y el commit del repositorio; `analyze.py` copia el modelo de CPU a `results.md`
  para la sección experimental del paper.
* El LP (compactación y compactación final con márgenes) se resuelve con SoPlex 8.1, llamado directamente.
* Todas las corridas usan el código actual (con Qhull para envolventes de muchos puntos y soluciones en el sistema
  de coordenadas de la malla original, ESICUP); en las instancias poliédricas del paper Qhull no se activa.
* `data/tollenaere2025_reference.csv`: mejores cotas superior e inferior de los modelos MILP de Tollenaere et al.
  (tablas B.1–B.4 del artículo) y si el óptimo está probado.
* Las mallas de Chess (licencia GPL) no están en el repositorio: `setup.sh` las descarga de ESICUP
  (`tools/fetch_lamas.sh`) y genera las descomposiciones con `tools/decompose_mesh.py`.
* Se certifican (aritmética racional exacta, `tools/certify_solution.py`) las soluciones de la tabla principal y de
  Liu; las de Chess se verifican con `PLP_check` sobre la descomposición conservadora (la propia corrida informa
  `feasible`).
