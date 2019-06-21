#!/usr/bin/env python
# coding: utf-8

# In[1]:


def readExcel(path_file):
    xlsx = pd.ExcelFile(path_file)
    
    try:
        pieces = pd.read_excel(xlsx, usecols = ['Pieces'])
        pieces = pd.DataFrame(pieces, dtype = np.int64).fillna(-1).to_numpy().ravel()

        data = pd.read_excel(xlsx, usecols = ['Length', 'Width', 'Height', 'Cube', 'Weight'])
        data_frame = pd.DataFrame(data)
        
        density = pd.read_excel(xlsx, usecols = ['Density'])
        density = pd.DataFrame(density)
    except:
        pieces = pd.read_excel(xlsx, usecols = ['PIECES'])
        pieces = pd.DataFrame(pieces, dtype = np.int64).fillna(-1).to_numpy().ravel()

        data = pd.read_excel(xlsx, usecols = ['LENGTH', 'WIDTH', 'HEIGHT', 'CUBE', 'WEIGHT'])
        data_frame = pd.DataFrame(data)
        
        density = pd.read_excel(xlsx, usecols = ['DENSITY'])
        density = pd.DataFrame(density)
        pass
        
    data_without_nan = pd.DataFrame(data_frame.dropna(), dtype = np.int64)#.to_numpy()
    data_with_nan = pd.DataFrame(data_frame.fillna(-1), dtype = np.int64)#.to_numpy()
    return pieces, data_with_nan, data_without_nan, density


# In[2]:


def add_pieces(pieces, data_with_nan, data_without_nan):
    for i in range(0, len(pieces)):
        if pieces[i] > 1:
            for j in range(0, pieces[i]):
                #print(data_with_nan[i])
                if data_with_nan.loc[i][0] < 0 or data_with_nan.loc[i][1] < 0 or data_with_nan.loc[i][2] < 0:
                    data_with_nan.loc[len(data_with_nan)] = data_with_nan.loc[i]
                else:
                    data_with_nan.loc[len(data_with_nan)] = data_with_nan.loc[i]
                    data_without_nan.loc[len(data_without_nan)] = data_without_nan.loc[i]
            pieces[i] = 1
    return pieces, data_with_nan, data_without_nan


# In[3]:


def box_generator(data_with_nan, data_without_nan):
    box_features = data_with_nan.iloc[[randint(0, len(data_with_nan)-1)]]
    if box_features.iloc[0][0] < 0 or box_features.iloc[0][1] < 0 or box_features.iloc[0][2] < 0:
        #print('Generando dimensiones')
        random_value = randint(0, len(data_without_nan)-1)
        for i in range(0, 3):
            box_features.iloc[0][i] = int(((box_features.iloc[0][4]**(1/3))*data_without_nan.iloc[random_value][i])/(data_without_nan.iloc[random_value][4])**(1/3))
    #else:
        #print('Las dimensiones ya existen')
    return box_features


# In[4]:


def =(array):
    #1 cubic feet = 1.728 cubic inch
    #1 inch = 2.56 cm
    #1 liber = 2.20462 kg
    for i in range(0, 3):
        array[i] = np.ceil(array[i]*2.56)
    array[3] = array[3]*1.728*2.56
    array[4] = np.ceil(array[4]*2.20462)
    return array


# In[5]:


def exportTxt(dir_name, bin_L, bin_W, bin_H, list_instances):
    output = str(len(list_instances)) + '\n'
    for i in range(0, len(list_instances)):
        #instance number
        output += str(i+1) + ' 0 \n'
        #bin dimensions
        output += str(bin_L) + ' ' + str(bin_W) + ' ' + str(bin_H) + '\n'
        #number type boxes
        output += str(len(list_instances[i])) + '\n'
        for key in list_instances[i]:
            ids = str(key) #box id
            features = list(list_instances[i][key][0])
            l = str(features[0]) #box large
            rX = '1' #box rotation axi X
            w = str(features[1]) #box width
            rY = '1' #box rotation axi Y
            h = str(features[2]) #box height
            rZ = '1' #box rotation axi Z
            n = str(list_instances[i][key][1]) #box quantity
            W = str(features[4]) #box weight
            output += ids + ' ' + l + ' ' + rX + ' ' + w + ' '+ rY + ' ' + h + ' ' + rZ + ' ' + n + ' ' + W + '\n'
    file = open(dir_name + '.txt','w')
    file.write(output)
    file.close()
    print('Instancias almacenadas en:', os.getcwd(), '.\n')


# In[6]:


def instance_generator(max_vol, max_weight, number_instances):
    instances = []
    print('Generando instancias')
    for i in range(0, number_instances):
        print('.', end = '')
        key = 1
        boxes = {}
        box_features = imperial_to_metric(box_generator(data_with_nan, data_without_nan).to_numpy().ravel())
        boxes[key] = [box_features, 1]
        volume = box_features[3]
        weight = box_features[4]
        same_features = False
        while volume < 2 * max_vol and weight < 2 * max_weight:
            box_features = imperial_to_metric(box_generator(data_with_nan, data_without_nan).to_numpy().ravel())
            for i in boxes:
                features = boxes[i][0]
                if len(np.setdiff1d(features, box_features)) == 0:
                    same_features = True
                    break
            if same_features == True:
                #print('igual')
                boxes[i][1] += 1
            else:
                #print('diferente')
                key += 1
                boxes[key] = [box_features, 1]
            volume += box_features[3]
            weight += box_features[4]
        instances.append(boxes)
        volume = 0
        weight = 0
    print()
    return instances


# In[7]:


def stadistic_densities(pieces, density):
    density = density.to_numpy()
    data = []
    for i in range(0, len(pieces)):
        if pieces[i] > 1:
            for j in range(0, pieces[i]):
                data.append(density[j])
        else:
            data.append(density[i])
    return np.mean(density), np.std(density)


# In[11]:


def exportMeansAndStd(means_stds):
    data = 'File\tArithmetic mean\tStandard deviation\tStd/Mean\tTotal items\n'
    for i in means_stds:
        for j in i:
            data += str(j) + '\t'
        data += '\n'
    file = open('means_and_stds.txt','w')
    file.write(data)
    file.close()
    print('Medias y Desviaciones almacenadas en:', os.getcwd(), '.\n')


# In[9]:


import os
import copy
import pandas as pd
import numpy as np
import collections as col

from random import seed
from random import randint


# In[10]:


seed(0)
DATADIR = "spreedsheets/OK/"
means_stds = []
for excel in os.listdir(DATADIR):
    if (".xls" in excel or ".xlsx" in excel) and '~$' not in excel:
        print('Leyendo archivo Excel:', excel, '.')
        pieces, data_with_nan, data_without_nan, density = readExcel(DATADIR + excel)
        print('Lectura de archivo Excel terminado.')
        print('Comenzando a agregar piezas repetidas.')
        pieces, data_with_nan, data_without_nan = add_pieces(pieces, data_with_nan, data_without_nan)
        print('Agregar piezas repetidas terminado.')
        print('Calculando media y desviacion estandar densidades')
        mean, std = stadistic_densities(pieces, density)
        print('Calculo terminado')
        means_stds.append([excel, mean, std, std/mean, np.sum(pieces)])
        
        bin_L = 1211 #cm
        bin_W = 234 #cm
        bin_H = 239 #cm
        max_vol = bin_L * bin_W * bin_H
        max_weight = 26730 #kg
        number_instances = 100

        instances = instance_generator(max_vol, max_weight, number_instances)
        print('Instancias generadas.')
        exportTxt(excel.split('.')[0], bin_L, bin_W, bin_H, instances)
        
exportMeansAndStd(means_stds)

