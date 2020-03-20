import os
import sys
import numpy as np
import plotly.graph_objects as go
import networkx  as nx

class nodo(object):
    def __init__(self, id_bin, group):
        super(nodo,self).__init__()
        self.__id_group = group
        self.__id_nodo = id_bin

    def getId(self):
        return self.__id_nodo

    def getGroup(self):
        return self.__id_group


class grafo(object):
    def __init__(self,nodes_cant, ngroups):
        super(grafo,self).__init__()
        self.__nodes_size = nodes_cant
        self.__groups_size = ngroups
        self.__transicion = np.zeros(
            self.getGroupSize, self.getNodesSize
            )
        self.__nodes = []

    def getNodesSize(self):
        return self.__nodes_size

    def getGroupSize(self):
        return self.__groups_size

    def setPositionTransicion(self, size_x, size_y, val):
        self.__transicion[size_x,size_y] = val
        self.__transicion[size_y,size_x] = val

    def add_node(self, new_node):
        self.__nodes.append(new_node)

if __name__ == "__main__":
    instances_file = sys.argv[1]                                                    #Se lee el archivo de instancias
    total_bins = 0                                                                  #TOTAL BINS OF THE ALGORITHM
    cant_group = 0                                                                  #TOTAL OF GROUPS ADDING 
    acumulated = 0                                                                  
    group_size = dict()
    groups_acumulated = dict()

    if(os.path.isfile(instances_file)):
        instancia = open(instances_file,'r')
        linea_actual = 0

        for linea in instancia:
            linea = linea.strip().split(" ")

            if (linea_actual == 0): #primera linea
                total_bins = int(linea[0])
                group_size = int(linea[1])

            elif (linea_actual <= cant_group):  #linea de los grupos
                size_actual_group = int(linea[1])   
                acumulated = acumulated + int(linea[1])
                group_size[int(linea[0])] = size_actual_group
                groups_acumulated[int(linea[0])] = acumulated

            else: #comparacion de cada bin
                first_bin_group = int(linea[0])
                bin_first_set = int(linea[1])
                bin_second_set = int(linea[2])
                value_to_compare = int(linea[3])
                nuevo = nodo(int(linea[1]),int(linea[0]))
                
            linea_actual += 1
            print("listo")
    else:
        print("FILE DONT EXIST")