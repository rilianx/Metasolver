'''
	A tener en consideracion cuando se usa, este script esta preparado para 
	entregar consultas de a una, por lo que si se quiere modificar el comportamiento
	para recibir lotes hay que modificar el parametro "batch" por en numero de 
	consultas por lote
'''

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
def initModel(model_yml = "/home/braulio/Metasolver/problems/clp/plugins/ann/model.yaml", weights = "/home/braulio/Metasolver/problems/clp/plugins/ann/model.h5"):
	# Ruta real: /home/braulio/Metasolver/problems/clp/plugins/ann/model.yaml
	global model
	if os.path.isfile(model_yml):
		file = open(model_yml)
		yaml_model = file.read()
		file.close()
		model = k.models.model_from_yaml(yaml_model)
		print "[Python] yml cargado" 
	else:
		print "[Python] No se pudo cargar el modelo"

	if os.path.isfile(weights):
		model.load_weights(weights)
		print "[Python] pesos cargados"
	else:
		print "[Python] No se pudo cargar los pesos"

	model.compile(loss = [loss], loss_weights = [0.2], optimizer = opt, metrics = met)


#Comprobando si modelo y pesos existen
def exists(modelFile, weights):
	if os.path.isfile(modelFile):
		if os.path.isfile(weights):
			return True
	return False

'''
	Permite modificar el tamano de las matrices de entradas de la red
'''
def setInputs(b = 1, x = 1,y = 1):
    global batch
    global input1
    global input2

    batch = b

    input1 = np.zeros((batch, x, y, 1))
    input2 = np.zeros((batch, x, y, 1))

#inicializacio del modelo
print "[Python] Iniciando modelo"
initModel()

'''
	Recibe la matrices de entrada para la ANN y retorna la lista de respuesta
	@param 	input1	primer parametro de entrada a la red, contiene la fotografia
					del contenedor desde arriba

	@param input2 	segundo paramentro de entrada, contiene el espacio que debe
					ser llenado alineado al primer parametro
'''
def getPrediction():
	global input1
	global input2
	global prediction
	global model

	#prediction es un array de numpy

	prediction = model.predict([np.array(input1), np.array(input2)], batch_size = 1)
	prediction = np.reshape(prediction, (batch,5,5,5))
	


#Modifica el valor de una celda del input 1
def setValueInput1(x, y, value, b = 0):
    global input1
    input1[b][x][y] = value


#Modifica el valor de una celda del input 2
def setValueInput2(x, y, value, b = 0):
    global input2
    input2 [b][x][y] = value

# Setea a cero (0) los valores de input2 que hayan sido modificados 
def resetInput2():
    global input2
    np.place(input2, input2!= 0, 0)


#obtiene el valor de una respuesta especifica en el cubo de respuestas
def getValue(x,y,z, b = 0):
	global prediction
	return prediction.item((b,x,y,z))