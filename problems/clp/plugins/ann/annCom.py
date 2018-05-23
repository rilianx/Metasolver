'''
	A tener en consideracion cuando se usa, este script esta preparado para
	entregar consultas de a una, por lo que si se quiere modificar el comportamiento
	para recibir lotes hay que modificar el parametro "batch" por en numero de
	consultas por lote
'''

f=open("hola.txt","w")
f.write("hola")
f.close()

import keras as k
import numpy as np
import os

'''
Parametros
'''





loss = "categorical_crossentropy"
opt = "adam"
met = ["accuracy"]


defaultX = 587
defaultY = 233

batch = 1

'''
	Valores importantes
'''
model = None
input1 = np.zeros((1,defaultX,defaultY,1))
input2 = np.zeros((1,defaultX,defaultY,1))
prediction = np.zeros((1,5,5,5))


'''
	Inicializa y compila la ANN a partir de un modelo y pesos entregados
'''
def initModel(model_yml = "model.yaml", weights = "model.h5"):

    global model
    if exists(model_yml, weights):
        file = open(model_yml)
        yaml_model = file.read()
        file.close()

        model = k.models.model_from_yaml(yaml_model)
        model.load_weights(weights)

        model.compile(loss = loss, optimizer = opt, metrics = met)


#Comprobando si modelo y pesos existen
def exists(modelFile, weights):
    if os.path.isfile(modelFile):
        if os.path.isfile(weights):
            return True
    return False

'''
	Permite modificar el tamanno de las matrices de entradas de la red
'''
def setInputs(b = 1, x = 1,y = 1):
    global batch
    global input1
    global input2

    batch = b

    input1 = np.zeros((batch, x, y, 1))
    input2 = np.zeros((batch, x, y, 1))

#inicializacio del modelo
initModel()

'''
	Recibe la matrices de entrada para la ANN y retorna la lista de respuesta
	@param 	input1	primer parametro de entrada a la red, contiene la fotografia
					del contenedor desde arriba

	@param input2 	segundo paramentro de entrada, contiene el espacio que debe
					ser llenado alineado al primer parametro
'''
def getPrediction():

    # TODO: generar input2

    global prediction
    global model

    #prediction es un array de numpy

    prediction = model.predict([np.array(input1), np.array(input2)], batch = 1)
    prediction = np.reshape(batch,5,5,5)


#Modifica el valor de una celda del input 1

def setValueInput1(b, x, y, value):
    global input1
    np.put(input1, ([b,x,y,0]), [value])


#Modifica el valor de una celda del input 2
def setValueInput2(b, x, y, value):
    global input2
    np.put(input2, ([b,x,y,0]), [value])


#obtiene el valor de una respuesta especifica en el cubo de respuestas
def getValue(b,x,y,z):
    global prediction
    return prediction.item((b,x,y,z))


'''
# Actualiza las dimensiones del input 2
def updateIn2(cl, cw, bx, by, bz, bl, bw, bh):

    # Obteniendo nextPlaceBox
    b = self.next_box

    # Buscando limites
    maxX = self.l
    maxY = self.w
    maxZ = b[2]

    # Recorriendo desde la X primero
    for x in range(b[0], maxX):
        y = b[1]
        if self.used[x][y] > maxZ:
            #print "Saliendo: ", self.used[x][y], ">", maxZ
            if maxX > x:
                maxX = x
            break
        for y in range(b[1], maxY):
            if self.used[x][y] > maxZ:
                if maxY > y:
                    maxY = y
                y = y - 1
                break

    # Y primero
    maxX2 = self.l
    maxY2 = self.w
    for y in range(b[1], maxY2):
        x = b[0]
        if self.used[x][y] > maxZ2:
            #print "Saliendo: ", self.used[x][y], ">", maxZ2
            if maxY2 > y:
                maxY2 = y
            break
        for x in range(b[0], maxX2):
            if self.used[x][y] > maxZ2:
                if maxX2 > x:
                    maxX2 = x
                x = x - 1
                break

    # Buscando limites a la inversa
    minX = 0
    minY = 0
    maxZ3 = b[2]

    # Recorriendo desde la X primero
    for x in range(b[0] + b[3] - 1, minX, -1):
        y = b[1] + b[4] - 1
        if self.used[x][y] > maxZ3:
            #print "Saliendo: ", self.used[x][y], ">", maxZ3
            if minX < x:
                minX = x
            break
        for y in range(b[1] + b[4] - 1, minY, -1):
            if self.used[x][y] > maxZ3:
                if minY < y:
                    minY = y
                y = y - 1
                break

    # Buscando limites a la ivnersa con Y primero
    minX2 = 0
    minY2 = 0
    maxZ3 = b[2]

    # Recorriendo desde la X primero
    for y in range(b[1] + b[4] - 1, minY2, -1):
        x = b[0] + b[3] - 1
        if self.used[x][y] > maxZ3:
            #print "Saliendo: ", self.used[x][y], ">", maxZ3
            if minY2 < y:
                minY2 = y
            break
        for x in range(b[0] + b[3] - 1, minX2, -1):
            if self.used[x][y] > maxZ3:
                if minX2 < x:
                    minX2 = x
                x = x - 1
                break

    area1 = getArea(b[0], b[1], maxX, maxY)
    area2 = getArea(b[0], b[1], maxX2, maxY2)
    area3 = getArea(minX, minY, b[0] + b[3], b[1] + b[4])
    area4 = getArea(minX2, minY2, b[0] + b[3], b[1] + b[4])
    maxa = max(area1, area2, area3, area4)
    bxlist = [b[0], b[1], b[0] + b[3], b[1] + b[4]]

    if maxa == area1 and rectInRect([b[0], b[1], maxX, maxY], bxlist):
        if rectInRect([minX, minY, maxX, maxY], bxlist):
            self.in2_dimensions = [minX, minY, maxX, maxY]
        elif rectInRect([minX2, minY2, maxX, maxY], bxlist):
            self.in2_dimensions = [minX2, minY2, maxX, maxY]
        else:
            self.in2_dimensions = [b[0], b[1], maxX, maxY]
        #print "in2_dimensions (OP1): ", self.in2_dimensions
        self.in2_height = self.h - b[2]
    elif maxa == area2 and rectInRect([b[0], b[1], maxX2, maxY2], bxlist):
        if rectInRect([minX, minY, maxX, maxY], bxlist):
            self.in2_dimensions = [minX, minY, maxX2, maxY2]
        elif rectInRect([minX2, minY2, maxX, maxY], bxlist):
            self.in2_dimensions = [minX2, minY2, maxX2, maxY2]
        else:
            self.in2_dimensions = [b[0], b[1], maxX2, maxY2]
        #print "in2_dimensions (OP2): ", self.in2_dimensions
        self.in2_height = self.h - b[2]
    elif maxa == area3 and rectInRect([minX, minY, b[0] + b[3], b[1] + b[4]], bxlist):
        self.in2_dimensions = [minX, minY, b[0] + b[3], b[1] + b[4]]
        #print "in2_dimensions (OP3): ", self.in2_dimensions
        self.in2_height = self.h - b[2]
    elif rectInRect([minX2, minY2, b[0] + b[3], b[1] + b[4]], bxlist):
        self.in2_dimensions = [minX2, minY2, b[0] + b[3], b[1] + b[4]]
        #print "in2_dimensions (OP4): ", self.in2_dimensions
        self.in2_height = self.h - b[2]
    else:
        if rectInRect([b[0], b[1], maxX, maxY], bxlist):
            self.in2_dimensions = [b[0], b[1], maxX, maxY]
            #print "in2_dimensions (OP5): ", self.in2_dimensions
            self.in2_height = self.h - b[2]
        elif rectInRect([b[0], b[1], maxX2, maxY2], bxlist):
            self.in2_dimensions = [b[0], b[1], maxX2, maxY2]
            #print "in2_dimensions (OP5): ", self.in2_dimensions
            self.in2_height = self.h - b[2]
        elif rectInRect([minX, minY, maxX, maxY], bxlist):
            self.in2_dimensions = [minX, minY, maxX, maxY]
            #print "in2_dimensions (OP5): ", self.in2_dimensions
            self.in2_height = self.h - b[2]
        elif rectInRect([minX, minY, maxX2, maxY2], bxlist):
            self.in2_dimensions = [minX, minY, maxX2, maxY2]
            #print "in2_dimensions (OP5): ", self.in2_dimensions
            self.in2_height = self.h - b[2]
        elif rectInRect([minX, minY, b[0] + b[3], b[1] + b[4]], bxlist):
            self.in2_dimensions = [minX, minY, b[0] + b[3], b[1] + b[4]]
            #print "in2_dimensions (OP3): ", self.in2_dimensions
            self.in2_height = self.h - b[2]
        elif rectInRect([minX2, minY2, b[0] + b[3], b[1] + b[4]], bxlist):
            self.in2_dimensions = [minX2, minY2, b[0] + b[3], b[1] + b[4]]
            #print "in2_dimensions (OP3): ", self.in2_dimensions
            self.in2_height = self.h - b[2]
        else:
            self.in2_dimensions = bxlist
            self.in2_height = self.h - b[2]
'''
