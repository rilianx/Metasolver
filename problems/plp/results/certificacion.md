# Certificación exacta de las soluciones

`tools/certify_solution.py` verifica una solución sin tolerancias: los vértices de las partes y las traslaciones
son doubles (racionales exactos) y todo se calcula con `fractions.Fraction`, incluida la alineación al mínimo
(itemOrigin = AlignToMinimum). Se verifica la contención y, para cada par de partes convexas cercanas, un eje
separador (normales de caras y productos cruz de aristas, en racionales) con `max_A n·a <= min_B n·b`.

Por qué no basta el LP: la compactación deja piezas en contacto con errores de ~1e-14 (tolerancias de SoPlex y
NFP calculados en punto flotante); los verificadores con tolerancia (1e-9) las aceptan, pero en aritmética
exacta hay solapamientos de ese orden. El certificado de Neumaier–Shcherbina tampoco lo resuelve: certifica una
cota del valor óptimo del LP, no que el punto entregado cumpla las restricciones.

Solución (`BSG_PLP`, al final):
1. **Separación con margen** (`--margin`, 1e-7): una compactación por LP con `n·(t_i − t_j) ≥ d + m_k` y
   `x_i + w_i ≤ W − e_x` (y en y), holguras `m_k, w_i ∈ [0, margin]` que se maximizan sin subir la altura.
   Las holguras son suaves porque en filas que llenan exactamente el contenedor no cabe ningún margen (en las
   instancias de Stoyan, con dimensiones enteras, es frecuente). Una formulación con coeficientes de 1e-7 en
   la matriz hacía fallar a SoPlex; con las holguras como variables acotadas en [0, margin] y coeficiente 1 no.
2. **Redondeo a múltiplos de 2^-30** (operaciones exactas en doubles): en esas filas la posición de contacto
   suele ser una suma de dimensiones (p. ej., 27 en vez de 27.000000000000004); el error (< 1e-9) es menor que
   el margen de los pares separados. Se acepta solo si `Solution::verify` la declara factible.

Resultado (20 s, semillas 1 y 2, 11 instancias: Stoyan 2005 Ex1–3, Stoyan 2004 Ex1–5, Merged1, 3, 5):

| Versión | Certificadas |
|---|---|
| sin margen | 0 de 3 probadas (solapamientos de ~1e-14) |
| margen con holguras de coeficiente 1e-7 | 18 de 22 (el LP falló en 4) |
| margen con holguras acotadas | 17 de 22 (5 con contactos forzados, solapamiento ≤ 1.8e-14) |
| margen + redondeo a 2^-30 | **22 de 22** |

La altura sube ~1e-7–1e-6. Las soluciones guardadas en `solutions/` son anteriores y no están certificadas
(verificadas con tolerancia 1e-9).
