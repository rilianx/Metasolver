//
// Created by braulio on 18-04-18.
//

#ifndef PROJECT_GLUE_H
#define PROJECT_GLUE_H

#endif //PROJECT_GLUE_H

#include <python2.7/Python.h>

typedef struct{
    PyObject *pName;
    PyObject *pModule;
}PyHandler;

/**
 * Crea un handler con los valores inicializados en null
 * @return
 */
PyHandler glue_createPyHandler();

/**
 * Inicializa el PyHandler y prepara la red para operar
 * @param pyHandler
 * @param pathScript
 * @param nameScript
 * @return retorna 1 si se pudo inicializar correctamente y 0 si hubo algun error
 */
int glue_initPyHandler(PyHandler pyHandler, const char *pathScript,
                       const char *nameScript);

/**
 * Ingresa valores de la entrada 1 a la red
 * @param pyHandler      - Handler inicializado con un script y modulo de python
 * @param nomrbeFuncion  - Nombre que tiene la funcion dentro del script de ejecucion
 *                         de la red (Python) para actualizar valores de la entrada.
 * @param x              - Coordenada X del imput que se quiere modificar
 * @param y              - Coordenada Y del imput que se quiere modificar
 * @param value          - Valor que se le quiere dar a la matriz en la coordenada (x,y)
 * @return retorna 1 si se ejecuto la funcion correctamente, en caso contrario retorna 0
 */
int glue_putInput(PyHandler pyHandler, const char *nomrbeFuncion, long x, long y, long value);


/**
 * Le pide a la red que opere las entradas y prepare una salida
 * @param pyHandler
 */
void glue_operate(PyHandler pyHandler);

/**
 * Obtiene la solucion de esa coordenada (del cubo)
 * @param pyHandler
 * @param x
 * @param y
 * @param z
 * @return  retorna la solucion generada por la red en las coordenadas (x,y,z)
 */
double glue_getSolution(PyHandler pyHandler, long x, long y, long z);
