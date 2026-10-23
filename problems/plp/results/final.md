# Resultados finales (paper)

Versión por defecto, 120 s, semillas 1–10, 4 corridas en paralelo (Xeon 2.8 GHz). Alturas finales con el margen
de certificación. Brechas respecto de la mejor altura publicada (tabla del paper).

```
## Tabla principal (full, 120 s)
Stoyan_et_al._2005__Example1     n=10 media 17.47 mejor 17.46  sd 0.02  infactibles 0
Stoyan_et_al._2005__Example2     n=10 media 19.13 mejor 19.13  sd 0.00  infactibles 0
Stoyan_et_al._2005__Example3     n=10 media 27.48 mejor 27.00  sd 0.31  infactibles 0
Stoyan_et_al._2004__Example1     n=10 media 52.95 mejor 52.23  sd 0.53  infactibles 0
Stoyan_et_al._2004__Example2     n=10 media 29.00 mejor 29.00  sd 0.00  infactibles 0
Stoyan_et_al._2004__Example3     n=10 media 39.95 mejor 38.65  sd 0.61  infactibles 0
Stoyan_et_al._2004__Example4     n=10 media 53.20 mejor 52.46  sd 0.59  infactibles 0
Stoyan_et_al._2004__Example5     n=10 media 66.33 mejor 64.96  sd 0.88  infactibles 0
Egeblad_et_al._2009__Merged1     n=10 media 16.66 mejor 16.57  sd 0.06  infactibles 0
Egeblad_et_al._2009__Merged2     n=10 media 21.01 mejor 21.00  sd 0.01  infactibles 0
Egeblad_et_al._2009__Merged3     n=10 media 22.38 mejor 22.00  sd 0.27  infactibles 0
Egeblad_et_al._2009__Merged4     n=10 media 25.00 mejor 24.10  sd 0.66  infactibles 0
Egeblad_et_al._2009__Merged5     n=10 media 27.91 mejor 27.00  sd 0.31  infactibles 0

## Ablacion (brecha media)
full         S2004 -3.2%  S2005 -2.0%  S2004Ex1 52.95  (n=10)
nolp         S2004 -3.0%  S2005 -2.1%  S2004Ex1 52.87  (n=10)
add          S2004 -1.3%  S2005 -1.0%  S2004Ex1 53.23  (n=10)
swap         S2004 -3.2%  S2005 -2.0%  S2004Ex1 53.02  (n=10)
slow (24 s)  S2004 -0.5%  S2005 -1.0%  S2004Ex1 54.33  (n=10)
```

Certificación exacta (tools/certify_solution.py) de las 130 soluciones `full`: 129 certificadas; la restante
(Stoyan 2004 Ex3, semilla 8) tiene 3 contactos forzados con solapamiento <= 1.7e-15.

Mejores soluciones certificadas: `solutions_final/`.
