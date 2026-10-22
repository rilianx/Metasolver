# Ajuste de parámetros de BSG_PLP

Parámetros: `beta` (altura de la pieza), `gamma` (aumento de la altura), `slide` (empuje hacia las paredes),
`grid` (puntos de partida por eje) y `block_waste` (hueco máximo de los pseudobloques); `alpha` = 1 fijo (la
evaluación solo depende de las proporciones entre los coeficientes). Pseudobloques activos (30, hueco + volumen total).

* **Entrenamiento**: 16 instancias (Stoyan 2005 Ex2–3, Stoyan 2004 Ex2 y Ex4, Tollenaere STOYAN_2005 13/17/20,
  STOYAN_2004 12/16/19, DIAMONDS 7/9, TETRAHEDRONS 11/15/18), 10 s cada una. Medida: brecha media contra la mejor
  altura conocida.
* **Búsqueda aleatoria**: 30 configuraciones + la anterior, 1 semilla; las 6 mejores con 3 semillas. Ninguna superó
  a la configuración anterior.
* **Optuna** 5.0 (TPE multivariado, con las 31 evaluaciones anteriores como punto de partida; poda por mediana
  cada 4 instancias): 40 configuraciones nuevas (34 completas, 37 podadas en total); las 4 mejores con 3 semillas.
* **Validación**: las 58 instancias restantes (30 s las clásicas, 10 s las de Tollenaere), 3 semillas.

## Entrenamiento (3 semillas)

| Configuración | beta | gamma | slide | grid | block_waste | Brecha |
|---|---|---|---|---|---|---|
| o51 | 5.53 | 0.77 | 0.065 | 4 | 0.07 | +6.18% |
| o31 | 3.10 | 1.28 | 0.004 | 4 | 0.20 | +6.48% |
| o45 | 5.57 | 0.64 | 0.008 | 4 | 0.15 | +6.56% |
| anterior | 1 | 1 | 0.1 | 3 | 0.1 | +6.93% |

## Validación (58 instancias, brecha media de la media de 3 semillas)

| Grupo | anterior | o51 | o31 | o45 |
|---|---|---|---|---|
| Stoyan 2004 Ex3 y Ex5 | +25.84% | **+19.63%** | +23.17% | +20.10% |
| Stoyan 2005 Ex1 | +2.19% | **+0.02%** | +0.03% | +0.02% |
| T2025 DIAMONDS | **+0.49%** | +2.15% | +2.44% | +3.56% |
| T2025 STOYAN_2004 | +1.32% | +1.08% | +1.60% | **+0.98%** |
| T2025 STOYAN_2005 | **+3.97%** | +4.76% | +4.18% | +4.19% |
| T2025 TETRAHEDRONS | +0.43% | +0.48% | **+0.41%** | +0.49% |
| Todas | **+2.57%** | +2.68% | +2.80% | +2.68% |

Por instancia contra la anterior: o51 mejor en 16 y peor en 16; o31 y o45 mejores en 17 y peores en 14.

Instancias clásicas de validación (alturas de las 3 semillas):

| Instancia | anterior | o51 |
|---|---|---|
| Stoyan 2005 Ex1 (óptimo 17.46) | 17.84 / 17.84 / 17.84 | 17.46 / 17.46 / 17.46 |
| Stoyan 2004 Ex1 | 62.72 / 59.63 / 63.00 | 58.15 / 59.07 / 59.43 |
| Stoyan 2004 Ex3 (mejor conocida 41.30) | 54.59 / 54.59 / 53.93 | 52.00 / 52.00 / 52.00 |
| Stoyan 2004 Ex5 (mejor conocida 72.33) | 86.69 / 87.11 / 86.67 | 83.74 / 81.10 / 81.10 |

**Decisión**: o51 queda por defecto. Mejora claramente las instancias clásicas (las comparables con la literatura)
y alcanza el óptimo de Stoyan 2005 Ex1, pero empeora DIAMONDS (piezas con muchas caras, donde `grid 4` hace más
caro cada paso) y algo STOYAN_2005 de Tollenaere. Los valores anteriores se pueden usar con
`--beta 1 --gamma 1 --slide 0.1 --grid 3 --block_waste 0.1`.

El ajuste destapó dos errores, ya corregidos: excepciones internas de SoPlex no capturadas (el programa terminaba)
y búsquedas iniciales que no completaban una solución en su tiempo.
