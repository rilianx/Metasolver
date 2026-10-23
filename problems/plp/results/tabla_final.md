# Tabla final de comparación (borrador)

Alturas (menor es mejor). Versión por defecto actual; Stoyan: 120 s, Merged: 600 s (y valor a los 120 s, de los
logs), semillas 1–3, media (mejor). Soluciones verificadas con tolerancia 1e-9 (aún sin certificado exacto).

Referencias:
* **Exacta**: geometría exacta. Stoyan 2005 y Merged: 3DNest (Egeblad et al. 2009; 10 corridas de 600 s, Core 2
  de 2.16 GHz; mejor = utilización máxima, media = utilización media a los 600 s, convertidas a altura). Stoyan
  2005 Ex1: óptimo (Tollenaere et al. 2025). Stoyan 2004: St04 = Stoyan et al. 2004 (funciones phi).
* **Vóxeles**: Lamas-Fernández et al. (VNS; las piezas se sobreestiman, p. ej. +15 % de volumen en Merged1).
  Mejor: Stoyan, resolución estándar, 10 corridas de 1 h; Merged, resolución extra-fina, 4 h. Media: resolución
  estándar.

| Instancia | Piezas | Exacta mejor | Exacta media | Vóxeles mejor | Vóxeles media | Nuestra: media (mejor) |
|---|---|---|---|---|---|---|
| Stoyan 2005 Ex1 | 7 | 17.46 (óptimo, Tollenaere 2025) | — | — | — | 17.48 (17.46) |
| Stoyan 2005 Ex2 | 12 | 19.01 | 19.83 | 21.23 | 21.70 | 19.13 (19.13) |
| Stoyan 2005 Ex3 | 25 | 28.83 | 29.82 | 29.76 | 31.53 | 27.52 (27.00) |
| Stoyan 2004 Ex2 | 20 | 32 (St04) | — | 28.47 | 29.40 | 29.00 (29.00) |
| Stoyan 2004 Ex3 | 30 | 49 (St04) | — | 41.30 | 42.65 | 40.15 (40.00) |
| Stoyan 2004 Ex4 | 40 | 63.21 (St04) | — | 54.83 | 59.52 | 52.69 (50.91) |
| Stoyan 2004 Ex5 | 50 | 78.7 (St04) | — | 72.33 | 76.81 | 67.21 (66.54) |
| Merged1 | 15 | 17.00 | 17.45 | 17.13 (4 h) | 18.67 | 16.67 (16.61); 600 s: 16.60 (16.55) |
| Merged2 | 30 | 21.98 | 22.02 | 22.06 (4 h) | 22.64 | 21.00 (21.00); 600 s: 21.00 (21.00) |
| Merged3 | 45 | 24.04 | 24.97 | 24.32 (4 h) | 26.26 | 22.42 (22.40); 600 s: 22.07 (22.00) |
| Merged4 | 60 | 27.81 | 28.35 | 26.47 (4 h) | 29.44 | 24.67 (24.01); 600 s: 24.47 (24.01) |
| Merged5 | 75 | 30.13 | 31.31 | 29.24 (4 h) | 31.12 | 27.49 (27.00); 600 s: 27.04 (27.00) |

Notas:
* Stoyan 2005 Ex1 de 3DNest no se incluye: con nuestros volúmenes su utilización da alturas incompatibles con el
  óptimo (la instancia parece distinta). Stoyan 2004 Ex1 no tiene referencia comparable (Lamas usa el Exp1 de
  Liu et al. 2015, otra instancia).
* Stoyan 2005 Ex2–3 y Merged1–5 son la misma instancia exacta que 3DNest; en Stoyan 2004 la única referencia
  exacta es St04 (2004).
