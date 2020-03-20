import numpy as np
def CountNullBoxes(matrix):
    val = False
    boxes_null = list()
    for box in range(matrix.shape[1]): boxes_null.append(0)
    for bin in range(matrix.shape[0]):
        for box in range(matrix.shape[1]):
            if(matrix[bin][box]==1):
                boxes_null[box] = boxes_null[box] + 1
    for box in range(matrix.shape[1]):
        if(boxes_null[box] == 0):
            print("ERROR")
            val = True
    if(val): print("ERROR")
    return boxes_null

def MatrixGenerator(NombreArchivo):                                             #Se captura el nombre del archivo de lectura
    archivo = open(NombreArchivo,'r')
    Valores = dict()
    boxes_availables = list()
    num_cajas=0
    num_container=0
    linea_actual=0
    container=0
    maping_boxes = []

    for linea in archivo:                                                       #Se almacena cada container en un diccionario como un ID-KEY y arreglo con cada ID de la caja
        token = linea.strip("\n").split(" ")
        token = token[1:len(token)]
        if(linea_actual == 0):
            num_container = int(token[0])                                       #Almacena la cantidad de contenedores
            num_cajas = int(token[1])                                           #Almacena la cantidad total de cajas
            all_boxes = token[2:len(token)-1]
            #print(all_boxes)
        elif(linea_actual%2 != 0):
            container = int((linea_actual+1)/2)
            #print(container)                                                    #annadimos un id arbitrario, pues solo nos presentan la cantidad de cajas enumerado
        elif(linea_actual%2 == 0 and linea_actual > 0):
            Valores[container] = token                                          #Se annade el total de cajas de ese contenedor a la cantidad container
            #container = container+1
        linea_actual=linea_actual+1

    Matrix_numpy = np.zeros((num_container, num_cajas))                         #Matrix donde se cargara la cantidad de cajas por contenedor
    for box in range(num_cajas): boxes_availables.append(0)

    for bin in range(num_container):                                            #Las cajas y contenedores estan enumeradas desde 1 hasta num_container
        actual_len = Valores[bin+1]                                             #Se lee la fila actual con un determinado contenedor
        for box in range(num_cajas):                                            #Se procede a recorrer caja caja del contenedor
            pivote = str(box)                                                   #Solo nos interesa saber si existe la caja (j+1) dentro de la lectura
            if(actual_len.count(pivote)!=0):
                Matrix_numpy[bin,box] = 1
                boxes_availables[box] = 1

    dist = 0                                                                    #Valor dist que nos indica la cantidad de posicion a mover
    total_null = 0                                                              #total de cajas no consideradas y no presentes
    for id in boxes_availables:
        if (id==1): total_null = total_null+1

    #for box in all_boxes:
    #    index = int(box)
    #    boxes_availables[index] = 1
    #print(all_boxes)
    #print(len(all_boxes),len(boxes_availables))
                                                                                #Eliminamos Las cajas que no convenientes como las cajas que no se encuentran en la matrix
    interpretate_matrix = np.zeros((num_container, total_null))       #Matrix donde se hara un parsing de todos los elementos que no tengan determinadas cajas
    for index in range(num_cajas):
        pair_add = []
        if(boxes_availables[index] == 0):
            dist = dist+1
            pair_add=[index,-1]
        else:
            pair_add = [index,index-dist]
        maping_boxes.append(pair_add)

    for bin in range(num_container):
        #print(interpretate_matrix[bin])
        for box in range(num_cajas):
            if(boxes_availables[box] == 1):
                y = (maping_boxes[box])[1]
                interpretate_matrix[bin,y]= Matrix_numpy[bin,box]

    #print(boxes_availables)
    #print(interpretate_matrix)
    return interpretate_matrix, maping_boxes
