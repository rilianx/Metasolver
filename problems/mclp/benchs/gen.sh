#!/bin/bash

gcc -ansi -o 3dbpp -O3 test3dbpp.c 3dbpp.c -lm

mkdir class1
cd class1
../3dbpp 50 100 1 0 0 1 0
../3dbpp 100 100 1 0 0 1 0
../3dbpp 150 100 1 0 0 1 0
../3dbpp 200 100 1 0 0 1 0
cd ..

mkdir class4
cd class4
../3dbpp 50 100 4 0 0 1 0
../3dbpp 100 100 4 0 0 1 0
../3dbpp 150 100 4 0 0 1 0
../3dbpp 200 100 4 0 0 1 0
cd ..

mkdir class5
cd class5
../3dbpp 50 100 5 0 0 1 0
../3dbpp 100 100 5 0 0 1 0
../3dbpp 150 100 5 0 0 1 0
../3dbpp 200 100 5 0 0 1 0
cd ..

mkdir class6
cd class6
../3dbpp 50 10 6 0 0 1 0
../3dbpp 100 10 6 0 0 1 0
../3dbpp 150 10 6 0 0 1 0
../3dbpp 200 10 6 0 0 1 0
cd ..

mkdir class7
cd class7
../3dbpp 50 40 7 0 0 1 0
../3dbpp 100 40 7 0 0 1 0
../3dbpp 150 40 7 0 0 1 0
../3dbpp 200 40 7 0 0 1 0
cd ..

mkdir class8
cd class8
../3dbpp 50 100 8 0 0 1 0
../3dbpp 100 100 8 0 0 1 0
../3dbpp 150 100 8 0 0 1 0
../3dbpp 200 100 8 0 0 1 0
cd ..

mkdir class2
cd class2
../3dbpp 50 100 2 0 0 1 0
../3dbpp 100 100 2 0 0 1 0
../3dbpp 150 100 2 0 0 1 0
../3dbpp 200 100 2 0 0 1 0
cd ..

mkdir class3
cd class3
../3dbpp 50 100 3 0 0 1 0
../3dbpp 100 100 3 0 0 1 0
../3dbpp 150 100 3 0 0 1 0
../3dbpp 200 100 3 0 0 1 0
cd ..
