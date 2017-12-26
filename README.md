[![Build Status](https://travis-ci.org/rilianx/Metasolver.svg?branch=mop-bsg)](https://travis-ci.org/rilianx/Metasolver)
# Metasolver


Clonar con comando el comando
git clone https://github.com/rilianx/Metasolver.git

Compilar el proyecto con el comando
cmake .
Correr el programa con el siguiente comando
./BSG_CLP problems/clp/benchs/BR/BR8.txt 1 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0


./BSG_CLP problems/clp/benchs/BR/BR8.txt <inst_id> <min_fr> <maxtime> <alpha> <beta> <gamma> <p> <delta> <theta> <r> <fsb> <kdtree>

inst_id: id de la instancia

min_fr: minimo volumen necesario para construir bloques

alpha, beta, gamma, p, delta, theta, r: parametros de la funcion VCS 

fsb: variante full-supported

kdtree: uso de estructura kdtree

TODO
----

**Ariel:**
- [x] Hacer un generador de instancias siguiendo parametros del paper
- [ ] Resolver instancias generadas usando BSG (opcion fsb=1)
- [ ] Crear archivos con set de entrenamiento y pruebas (input/output de la red) (definir formato con Braulio)


**Ignacio:**
- [x] Ordenar path


