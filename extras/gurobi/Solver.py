from gurobipy import *
from GenerateMatrix import MatrixGenerator
import numpy as np
import sys
import os
import os.path
#Recibe como parametro el nombre del archivo a editar
NombreArchivo = sys.argv[1]
#Guarda la ruta de ambos archivos por si hay que cambiarlos en un futuro
almacen_archivos = ""
#Corrobora la existencia del archivo
if(os.path.exists(NombreArchivo)):
        #Genera la matrix para el archivo solicitado
        covertureMatrix, index_boxes =  MatrixGenerator(NombreArchivo)
        numberOfContainer=covertureMatrix.shape[0]
        numberOfBox=covertureMatrix.shape[1]
        print(numberOfContainer, numberOfBox)
        #create and setting model
        model= Model("Container Covering")
        model.setParam('OutputFlag', False)
        #set variables
        c={}
        solution2 = []
        solution=[]
        for i in range(numberOfContainer):
                c[i]=model.addVar(vtype=GRB.BINARY,name="c")

        #set objective
        model.setObjective(quicksum(c[a]*1 for a in range(numberOfContainer)),GRB.MINIMIZE)

        #set constraints
        for i in range(numberOfBox):
                model.addConstr(quicksum(covertureMatrix[j,i]*c[j] for j in range(numberOfContainer)) >= 1)
        #run model
        model.optimize()
        #return solution
        #print(model.X)
        for i in range(numberOfContainer):
                if(model.X[i]):
                        solution.append(i)
        print(solution)
        print(len(solution))
else:
        print("FILE NOT FOUND")
