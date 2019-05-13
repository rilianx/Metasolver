# encoding: utf-8
import matplotlib.pyplot as plt
import sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-state","--state",type = int, help = "Variable Number to show")
parser.add_argument("-interval","--interval",type = int, help = "Separation interval of the states" )
args = parser.parse_args()

var = 0
interval = -1
alpha_media = 0
if args.state:
    var = args.state
if args.interval:
    interval = args.interval

lectura = []
statesX = [[],[],[]]
statesY = [[],[],[]]

#reading each file with the number of states
for i in range(3):
    lectura.append(open("Values/State"+str(i)+"/Values"+str(var)+".txt",'r'))

#Making a list, that includes all the X values and Y values
for i in range(3):
    for linea in (lectura[i]).readlines():
        valores = linea.strip().split(" ")
        alpha_media = valores[0]
        statesX[i].append(float(valores[1]))
        statesY[i].append(float(valores[2]))

#Graphing the values
if(interval != -1):
    newX = [[],[],[]]
    newY = [[],[],[]]
    for i in range(3):
        for j in range(len(statesX[i])):
            if (j%interval==0):
                newX[i].append((statesX[i])[j])
                newY[i].append((statesY[i])[j])
    plt.plot(newX[0],newY[0],'o-')
    plt.plot(newX[1],newY[1],'o-')
    plt.plot(newX[2],newY[2],'o-')
else:

    plt.plot(statesX[0],statesY[0],'o-')
    plt.plot(statesX[1],statesY[1],'o-')
    plt.plot(statesX[2],statesY[2],'o-')

plt.grid()
plt.legend(('State 0', 'State 1', 'State 2'),prop = {'size':10}, loc = 'upper right')
plt.title("Variable "+str(var)+" with alpha_media = "+alpha_media)
plt.show()
