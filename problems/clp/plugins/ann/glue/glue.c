//
// Created by braulio on 18-04-18.
//

#include "glue.h"
#define DEB 0

PyObject *glue_initPyHandler(PyHandler pyHandler, const char *pathScript, const char *nameScript) {

    // Preparando Python
    Py_Initialize();
    //PySys_SetPath((char *) pathScript);

    // Importando modulos externos
    PyObject *pKerasStr = PyUnicode_FromString("keras");
    PyObject *pKerasMod = PyImport_Import(pKerasStr);
    PyObject *pNumpyStr = PyUnicode_FromString("numpy");
    PyObject *pNumpyMod = PyImport_Import(pNumpyStr); 

    PySys_SetPath((char *) pathScript);

    // Cargando script e importando modulo
    pyHandler.pName = PyString_FromString(nameScript);
    pyHandler.pModule = PyImport_ImportModule(nameScript);
    if(pyHandler.pModule == NULL || PyErr_Occurred()){
        printf("[initPyHandler] Error cargando %s\n", nameScript);
        PyErr_Print();
        return 0;
    }
    else{
        printf("[initPyHandler] Python initilized\n");
    }

    /*
     * NOTA: el inicializado de la red tiene que estar en la ejecucion del main del script
     * de esta forma se inicializa automaticamente cuando se carga el script. Por otro lado,
     * las entregas de entradas y peticiones de salidas de la red tienen que ser mediante
     * funciones.
     */

    return pyHandler.pModule;
}

int glue_putInput(PyHandler pyHandler, const char *nombreFuncion, long x, long y, long value) {
    // Transformacion de variables de C a Python
    PyObject *pX = PyInt_FromLong(x);
    PyObject *pY = PyInt_FromLong(y);
    PyObject *pValue = PyInt_FromLong(value);
    if(!pValue || !pX || !pY){
        printf("[glue_putInput] Error en la transformacion de variables\n");
        return 0;
    }

    // Preparando argumentos para la funcion
    PyObject *pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, pX);
    PyTuple_SetItem(pArgs, 1, pY);
    PyTuple_SetItem(pArgs, 2, pValue);

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);
    
    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_putInput] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");
     

    // Ejecutando funcion
    PyObject_CallObject(pFunc, pArgs);
    return 1;
}

void glue_operate(PyHandler pyHandler){
    /*
     * nomreFuncion es el nombre que tiene la funcion dentron del script de ejecucion
     * de la red (Python) para operar las entradas y generar una salida.
     */
    const char* nombreFuncion = "getPrediction";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);
    
    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_putInput] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion
    PyObject_CallObject(pFunc, NULL);
}

void glue_resetInput2(PyHandler pyHandler){
    /*
     * nomreFuncion es el nombre que tiene la funcion dentron del script de ejecucion
     * de la red (Python) para operar las entradas y generar una salida.
     */
    const char* nombreFuncion = "resetInput2";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);
    
    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_putInput-] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion
    PyObject_CallObject(pFunc, NULL);
}

void glue_fillInput2(PyHandler pyHandler){
    /*
     * nomreFuncion es el nombre que tiene la funcion dentron del script de ejecucion
     * de la red (Python) para operar las entradas y generar una salida.
     */
    const char* nombreFuncion = "fillInput2";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);
    
    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_putInput-] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion
    PyObject_CallObject(pFunc, NULL);
}


/*
  FIXME: funcion getValue queda fuera de uso desde acutalizacion pasada.
  ahora recibe alpha, beta, gamma y P con sus respectivas funciones get()
*/ 
double glue_getSolution(PyHandler pyHandler, long x, long y, long z) {
    /*
     * nomreFuncion es el nombre que tiene la funcion dentron del script de ejecucion
     * de la red (Python) para retornar la solucion que se encuentra en la coordenada
     * (x,y,z) dentro del cubo de solucion.
     */
    const char* nombreFuncion = "getValue";

    // Transformacion de variables de C a Python
    PyObject *pX = PyInt_FromLong(x);
    PyObject *pY = PyInt_FromLong(y);
    PyObject *pZ = PyInt_FromLong(z);

    if(!pX || !pY || !pZ)
        printf("[glue_getSolution] Error en la transformacion de variables");

    // Preparando argumentos para la funcion
    PyObject *pArgs = PyTuple_New(3);
    PyTuple_SetItem(pArgs, 0, pX);
    PyTuple_SetItem(pArgs, 1, pY);
    PyTuple_SetItem(pArgs, 2, pZ);

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);
    
    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_putInput] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion y obteniendo solucion
    PyObject *pRet = PyObject_CallObject(pFunc, pArgs);
    double solucion = PyFloat_AsDouble(pRet);

    return solucion;
}

double glue_getAlpha(PyHandler pyHandler){
    const char* nombreFuncion = "getAlpha";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);

    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_getAlpha] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion y obteniendo solucion
    PyObject *pRet = PyObject_CallObject(pFunc, NULL);
    
    double solucion = PyFloat_AsDouble(pRet);
}

double glue_getBeta(PyHandler pyHandler){
    const char* nombreFuncion = "getBeta";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);

    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_getBeta] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion y obteniendo solucion
    PyObject *pRet = PyObject_CallObject(pFunc, NULL);
    
    double solucion = PyFloat_AsDouble(pRet);
}

double glue_getGamma(PyHandler pyHandler){
    const char* nombreFuncion = "getGamma";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);

    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_getGamma] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion y obteniendo solucion
    PyObject *pRet = PyObject_CallObject(pFunc, NULL);
    
    double solucion = PyFloat_AsDouble(pRet);
}

double glue_getP(PyHandler pyHandler){
    const char* nombreFuncion = "getP";

    // Preparando funcion
    PyObject *pFunc = PyObject_GetAttrString(pyHandler.pModule, nombreFuncion);

    // Comprobando errores
    if(PyErr_Occurred())
        PyErr_Print();
    if(!pFunc) 
        printf("[glue_getP] Error no se pudo encontrar la funcion %s\n", nombreFuncion);
    if(pFunc && PyCallable_Check(pFunc) && DEB)
        printf("Todo ok\n");

    // Ejecutando funcion y obteniendo solucion
    PyObject *pRet = PyObject_CallObject(pFunc, NULL);
    
    double solucion = PyFloat_AsDouble(pRet);
}


