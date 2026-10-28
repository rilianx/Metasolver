# Soluciones de las corridas finales

Mejor solución de 10 semillas (120 s) de cada instancia del paper, con el método final (sin compactación por LP,
con mejora local, reinicios con una pieza en posición aleatoria), de las corridas en el servidor (`../servidor/`:
2× Xeon E5-2620 v4, 14 procesos). Todas son factibles según el verificador (`PLP_check`, prueba de eje separador entre
todas las partes) con la tolerancia por defecto, 1e-6; cuatro (Merged3, Merged4, Stoyan 2004 Ex1 y Ex4) tienen
penetraciones de hasta 9e-7 y no pasan con `--eps 1e-9`. Son las soluciones de la Tabla 2 del paper. Las de versiones anteriores están en el historial de
git y en `../solutions_v2/`.
