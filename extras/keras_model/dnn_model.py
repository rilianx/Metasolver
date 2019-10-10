#!/usr/bin/env python
# coding: utf-8

# # Definitive Nerual Network Model

# ### Import libraries

# In[6]:


import numpy as np
import os
import sys
import time
import tensorflow as tf
from keras.models import model_from_json
from scipy.stats import moment
from keras import backend as K
from keras.models import Sequential
from keras.layers import Flatten, Dense, Input, Activation
from sklearn.model_selection import train_test_split
from random import seed
seed(0)

#from tensorflow.python.client import device_lib
#print(device_lib.list_local_devices()[1])

#from keras import backend as K
#K.tensorflow_backend._get_available_gpus()


# ## Functions

# ### Import data

# In[14]:


#Format data:
#volumen
#nb boxes
#nb spaces
#volumenes de cajas
#L de cajas
#W de cajas
#H de cajas
#volumenes de espacios
#L de espacios
#W de espacios
#H de espacios
'''
0.56
17
12
0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.008183553 0.012394972 0.012394972 0.007118734 0.007118734 0.013391263
0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.18398637 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.12947189 0.1890971 0.1890971 0.1890971 0.1890971 0.11925043 0.11925043 0.081771721 0.081771721 0.14480409 0.14480409 0.14310051 0.14310051 0.16524702 0.16524702 0.11413969 0.11413969 0.10562181 0.10562181
0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.32618026 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.46351931 0.30042918 0.30042918 0.20600858 0.20600858 0.47639485 0.47639485 0.47639485 0.47639485 0.36051502 0.36051502 0.36480687 0.36480687 0.28755365 0.26609442 0.41630901 0.26609442 0.41630901 0.28755365
0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.13636364 0.21818182 0.21818182 0.31818182 0.31818182 0.21818182 0.21818182 0.31818182 0.31818182 0.13636364 0.13636364 0.13636364 0.13636364 0.28181818 0.30454545 0.28181818 0.44090909 0.30454545 0.44090909
0.0029395519 0.01872905 0.0086788733 0.0064324508 0.00650859 0.0093052023 0.0051087385 0.0098181034 0.0051756386 0.0086837255 0.0046923823 0.017404175
0.049403748 0.28960818 0.12265758 0.11925043 0.11413969 0.28960818 0.12265758 0.28449744 0.11754685 0.28960818 0.12265758 0.17206133
0.27727273 0.38636364 0.42272727 0.15909091 0.16818182 0.12272727 0.15909091 0.13181818 0.16818182 0.13181818 0.16818182 0.38636364
0.21459227 0.16738197 0.16738197 0.33905579 0.33905579 0.26180258 0.26180258 0.26180258 0.26180258 0.22746781 0.22746781 0.26180258
'''

def process_data():
    try:
        sample = sys.argv[1:]
        processed_data = np.array([])
        processed_data = np.append(processed_data, float(sample[0]))
        processed_data = np.append(processed_data, int(sample[1]))
        processed_data = np.append(processed_data, int(sample[2]))
        for i in range(3, 11):
            data = np.array(filter(None, sample[i].split(","))).astype('float32')
            for item in get_array_features(data):
                processed_data = np.append(processed_data, item)
    except:
        print("The data was not processed. Input error.")
    return processed_data

def get_array_features(feature):
    feature = np.array([np.mean(feature), moment(feature, moment=2), moment(feature, moment=3), moment(feature, moment=4), moment(feature, moment=5)])
    return (feature - np.amin(feature)) / (np.amax(feature) - np.amin(feature))


def root_mean_squared_error(y_true, y_pred):
    return K.sqrt(K.mean(K.square(y_pred - y_true), axis=-1))


with tf.device('/gpu:0'):
    DATADIR = "trainingset_allinfo/"
    
    processed_data = process_data()
    print('Imported data')
    print(processed_data)
    print('Ubicacion actual: ', os.getcwd())
    json_file = open(os.getcwd() + '/extras/keras_model/dnn_model_relu.json', 'r')
    loaded_model_json = json_file.read()
    json_file.close()
    loaded_model = model_from_json(loaded_model_json)
    
    # load weights into new model
    loaded_model.load_weights(os.getcwd() + '/extras/keras_model/dnn_model_relu.h5')
    print("Loaded model from disk")

    # evaluate loaded model on test data
    loaded_model.compile(loss='mae',
                      optimizer='adam',
                      metrics=['mse', root_mean_squared_error,'mae'])
    print(loaded_model.predict(np.array([processed_data]))[0][0])