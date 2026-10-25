# Soluciones de las corridas finales

Mejor solución de 10 semillas (120 s) de cada instancia del paper, de las corridas en el servidor
(`../servidor/`: 2× Xeon E5-2620 v4, SoPlex 8.1, commit 5e61fde), certificada en aritmética exacta con
`tools/certify_solution.py` (139 de las 140 soluciones de las corridas se certificaron). Son las soluciones de la
Tabla 2 del paper. `../solutions_v2/` tiene las de la versión anterior (ibex, 4 procesos), algunas con menor
altura (Stoyan 2005 Ex3 26.45, Stoyan 2004 Ex4 50.64, Liu 29.41); `../solutions/` las mejores de corridas
exploratorias (verificadas con tolerancia 1e-9).
