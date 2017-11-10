[![Build Status](https://travis-ci.org/rilianx/Metasolver.svg?branch=mop-bsg)](https://travis-ci.org/rilianx/Metasolver)
# Metasolver


Clonar con comando el comando
git clone https://github.com/rilianx/Metasolver.git

Compilar el proyecto con el comando
cmake .
Correr el programa con el siguiente comando
./CLP2017 ../data/BR15.txt 14 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0

Nota: Para que el programa funcione se debe tener en el directorio origen los directorios bullet-2.80-rev2531 y data


TODO
----

 - [ ] **(Mauricio)** Arreglar crowding distance!
 - [ ] **(Cristobal, Ignacio)** NSGA-II
 - [ ] Agregar restriccion de peso maximo (en funcion de evaluacion VLoss)
 - [ ] Calcular hipervolumen
 - [ ] Comparar con resultados e instancias de otros papers

**Beam Search** con función *alpha*:
 - [x] Crear evaluadorMOP que incorpore el segundo objetivo con parametro f.
 - [x] Modificar BSGMOP que considere valores de alpha de acuerdo al numero de
      beams.
 - [x] Probar greedy para instancias con pesos aleatorios con distintos valores
       de f. Objetivo: encontrar f que maximice promedio de los pesos.
 - [x] Definir mapeo lambda2->f
