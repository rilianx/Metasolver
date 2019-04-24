# encoding: utf-8
import matplotlib.pyplot as plt
#from math import *
#from numpy import *

value = True
while(value):
    var = int(input("Variable: "))
    estados = []
    for i in range(3):
        estados.append("Values/State"+str(i)+"/Values"+str(var)+".txt")

    state0 = open(estados[0], "r")
    state1 = open(estados[1], "r")
    state2 = open(estados[2], "r")
    y0 = []
    y1 = []
    y2 = []
    maximo = []
    max0 = 0
    max1 = 0
    max2 = 0
    #filling the variable values
    for linea in state0.readlines():
        y0.append(float(linea))
    maximo.append(len(y0))
    for linea in state1.readlines():
        y1.append(float(linea))
    maximo.append(len(y1))
    for linea in state2.readlines():
        y2.append(float(linea))
    maximo.append(len(y2))
    #searching the min of the values
    x = range(0,min(maximo))

    #Graphing the values
    plt.plot(x,y0[0:min(maximo)],x,y1[0:min(maximo)],x,y2[0:min(maximo)])
    plt.grid()
    plt.legend(('State 0', 'State 1', 'State 2'),prop = {'size':10}, loc = 'upper right')
    plt.show()
    #plt.show()
    print("Quieres intentar otro valor?")
    valor = int(input("Si(1) - No(0): "))
    if(valor == 0):
        value = False
