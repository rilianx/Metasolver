#!/usr/bin/env python3
"""
Instancias del 3D-BPP con las clases de Martello, Pisinger y Vigo (2000), segun la descripcion
del paper (no es el generador original test3dbpp.c, asi que las instancias no son las mismas de
la literatura, pero tienen la misma distribucion).

Clases 1-5: contenedor 100x100x100; cada caja es del tipo k de la clase con probabilidad 0.6 y
de cada uno de los otros cuatro tipos con probabilidad 0.1:
  tipo 1: w en [1,W/2],    h en [2H/3,H], d en [2D/3,D]
  tipo 2: w en [2W/3,W],   h en [1,H/2],  d en [2D/3,D]
  tipo 3: w en [2W/3,W],   h en [2H/3,H], d en [1,D/2]
  tipo 4: w en [W/2,W],    h en [H/2,H],  d en [D/2,D]
  tipo 5: w en [1,W/2],    h en [1,H/2],  d en [1,D/2]
Clase 6: contenedor 10, cajas en [1,10]; clase 7: contenedor 40, cajas en [1,35];
clase 8: contenedor 100, cajas en [1,100].

Formato: "n L W H" y una linea "l w h" por caja. Archivo c{clase}_{n}_{instancia}.txt
uso: gen_mpv.py <directorio>
"""
import os, random, sys

def box(rng, cls):
    if cls >= 6:
        B, hi = {6: (10, 10), 7: (40, 35), 8: (100, 100)}[cls]
        return B, [rng.randint(1, hi) for _ in range(3)]
    W = H = D = 100
    t = cls if rng.random() < 0.6 else rng.choice([k for k in range(1, 6) if k != cls])
    lo_hi = {
        1: [(1, W // 2), (2 * H // 3, H), (2 * D // 3, D)],
        2: [(2 * W // 3, W), (1, H // 2), (2 * D // 3, D)],
        3: [(2 * W // 3, W), (2 * H // 3, H), (1, D // 2)],
        4: [(W // 2, W), (H // 2, H), (D // 2, D)],
        5: [(1, W // 2), (1, H // 2), (1, D // 2)],
    }[t]
    return 100, [rng.randint(a, b) for a, b in lo_hi]

def main():
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    for cls in [1, 4, 5, 6, 7, 8]:
        for n in [50, 100, 150, 200]:
            for i in range(1, 11):
                rng = random.Random(1000000 * cls + 1000 * n + i)
                boxes = [box(rng, cls) for _ in range(n)]
                B = boxes[0][0]
                with open(os.path.join(out, f"c{cls}_{n}_{i}.txt"), "w") as f:
                    f.write(f"{n} {B} {B} {B}\n")
                    for _, d in boxes:
                        f.write(" ".join(map(str, d)) + "\n")

if __name__ == "__main__":
    main()
