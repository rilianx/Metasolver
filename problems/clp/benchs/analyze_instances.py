import sys
import numpy as np
import matplotlib as plib
import matplotlib.pyplot as plt
from scipy.stats import pearsonr
from matplotlib.pyplot import figure


plt.figure( figsize=[3.5,3.5] )
    
f=open(sys.argv[1], "r")
n = int(f.readline())


vols = []
weights = []
profits = []
boxes = 0
for i in range(n):
  f.readline(); f.readline(); f.readline()
  nn = int(f.readline())

  for x in range(nn):
    line = f.readline()
    atr = line.split()
    vol = float(atr[1])*float(atr[3])*float(atr[5])/1000000.0
    #if vol > 1.64: print(atr[1], atr[3], atr[5]) 
    boxes += float(atr[7])
    vols.append(vol)
    weight = float(atr[8]) #/1000000.0
    weights.append(weight)
    profits.append(float(atr[9]))


plt.scatter(vols,weights,s=3)
plt.xlabel('volume (m^3)')
plt.ylabel('weight (kg)')
plt.title(sys.argv[2])
plt.tight_layout()

plt.tight_layout()
plt.show()



print("Pearson's correlation (weights):", pearsonr(vols,weights))
#print("Pearson's correlation (profits):", pearsonr(vols,profits))
print("nb boxes:", boxes/n)
print("mean vol:", np.mean(vols))
print("min-max vol:", np.min(vols), np.max(vols))
print("std vol:", np.std(vols))
#print("profits:", np.mean(profits))

