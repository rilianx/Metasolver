import os
import sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#matplotlib.use('Agg')
import networkx  as nx
import argparse

def getColorbyGroup(id_bin, set_groups):
    color = ["green","red","yellow","blue","purple","pink","orange","grey"]
    #color = ["white","white","white","white","white","white","white","white"]
    for key in set_groups:
        lim_inf = (set_groups.get(key))[0]    
        lim_sup = (set_groups.get(key))[1]
        if(lim_inf <= id_bin and lim_sup >= id_bin):
            return color[key]
    return "white"

if __name__ == "__main__":
    instances_file = sys.argv[1]
    lim_base = 0.7                                                    #Se lee el archivo de instancias
    lim_top = 1.0
    try:
        lim_base = float(sys.argv[2])
    except:
        lim_base += 0

    try:
        lim_top = float(sys.argv[3])
    except:
        lim_top += 0
    
    if(os.path.isfile(instances_file)):    
        instancia = open(instances_file,'r')                            
        linea_actual = 0                                                            #Linea que se encuentra leyendo actualmente       
        total_bins = 0                                                              #Total de bins obtenidos
        cant_group = 0                                                              #Total de grupos agregados 
        group_size = dict()                                                         #Diccionaro de Id_grupo: Cantidad_de_nodos
        pivote_last_iteration = 0                                                   #pivote que guarda la iteracion final
        weights = []                                                                #[[nodo1,nodo2,peso]]

        #Aqui se recorre todo el archivo, guardando en TDA todas las interacciones entre los nodos
        for linea in instancia:
            linea = linea.strip().split(" ")
            if (linea_actual == 0): #primera linea
                total_bins = int(linea[0])
                cant_group = int(linea[1])

            elif (linea_actual <= cant_group):  #linea de los grupos
                set_actual_group = []
                set_actual_group.append(pivote_last_iteration)
                pivote_last_iteration += (int(linea[1])-1)
                set_actual_group.append(pivote_last_iteration)
                pivote_last_iteration += 1   
                group_size[int(linea[0])] = set_actual_group
            else: #comparacion de cada bin
                if(linea):

                    bin_first_set = int(linea[0])
                    bin_second_set = int(linea[1]) 
                    value_to_compare = float("{0:.5f}".format(float(linea[2])))
                    nuevo = [bin_first_set,bin_second_set,value_to_compare]
                    weights.append(nuevo)
            
            linea_actual += 1

        #Hasta ahora se guardan todos los nodos de la forma ((nodo1,nodo2,peso))    
        #Obtiene la primera columna de nodos
        nodes_x = []

        #contiene la segunda columna de nodos
        nodes_y = []

        #Guarda los valores de cada asociacion
        values = []

        #Guarda los elementos en pareja: valor
        to_labeling = dict()
        
        
        #Elegimos solo los valores que usamos a partir de nuestro criterio
        for interaction in weights:

            if(interaction[2] >= lim_base and interaction[2] <= lim_top):
                nodes_x.append(str(interaction[0]))
                nodes_y.append(str(interaction[1]))
                id_key = (str(interaction[0]),str(interaction[1]))
                to_labeling[id_key] = str(interaction[2])

                values.append(str(interaction[2]))
        
        #Calcula el promedio de cada bin
        higher_prom = 0.0
        higher_value = 0
        
        for id_search in range(total_bins):
            prom = 0.0
            counter = 0
            val = 0
            for rel in weights:
                
                if( id_search == rel[0] or id_search == rel[1] ):
                    val += rel[2]
                    counter += 1    
            prom = val/counter
            
            if(prom > higher_value):
                higher_value = prom
                higher_prom = id_search        
        print(" El mejor nodo es "+str(higher_prom)+" con un valor de "+str(higher_value))
        
        #Contiene solo los nodos que se utilizaran 
        all_nodes_used = list(set(nodes_x+nodes_y))       #contiene solo los bins que se usaran
        
        #Se usa para obtener el orden de los colores a partir de los nodos usados
        colors = []
        for to_paint in all_nodes_used:
            colors.append(getColorbyGroup(int(to_paint),group_size))
        
        
        #Parseo de informacion para generar grafo
        
        #Se Genera el par origen-destino
        df = pd.DataFrame({'source':nodes_x, 'target':nodes_y, 'values':values})
        
        #Se identifica cada nodo usado con su respectivo color
        carac = pd.DataFrame({'ID':all_nodes_used, 'myvalue':colors })
        
        #Se asocian los nodos y se ingresan en el grafo g
        g = nx.from_pandas_edgelist(df,'source','target',edge_attr='values',create_using=nx.Graph() )
        pos = nx.spring_layout(g, k=1)
        print(pos)
        carac= carac.set_index('ID')
        carac=carac.reindex(g.nodes())
        

        #Se divida los caminos y los nodos
        nx.draw(g ,with_labels = True ,pos = pos,
                node_color = carac['myvalue'] ,node_size = 500)   
        
        #Se imprime el contenido de cada camino
        nx.draw_networkx_edge_labels(g, width = 1.0, font_size = 8, pos = pos, 
                                    edge_labels = to_labeling)

        plt.show()
    
    else:
        print("FILE DONT EXIST")