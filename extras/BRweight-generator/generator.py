#!/usr/bin/python

import sys
import os
import re
import ntpath
import numpy as np
import random

def sample_trunc(mu, sig, bounds=None):
    sample = np.random.normal(mu, sig)
    while sample < bounds[0] or sample > bounds[1]:
      sample = np.random.normal(mu, sig)

    return sample

a = sys.argv[1]
b = sys.argv[2] 

path = 'problems/clp/benchs/BR'
output_path = 'problems/clp/benchs/BRwp-'+a+'-'+b ## BR con weight y profit

a=float(a)
b=float(b)

Dmax=753 # 20' container
mu = a*Dmax 
sigma = b*mu # mean and standard deviation

# Create target Directory if don't exist
if not os.path.exists(output_path):
    os.mkdir(output_path)
    print("Directory " , output_path ,  " Created ")
    

files = []
# r=root, d=directories, f = files
for r, d, f in os.walk(path):
    for file in f:
        if 'BR' in file:
            files.append(os.path.join(r, file))

for f in files:
   file = open(f, "r")
   file_name = ntpath.basename(f)
   file_out = open(output_path + '/' + file_name, "w")
   
   nb_instances = int(file.readline()) # number of instances
   file_out.write(str(nb_instances)+"\n")
   for i in range(0,nb_instances):
     file_out.write(file.readline()) # id_inst seed
     dimensions = file.readline().rstrip()
     dims = dimensions.split()
     L = int(dims[0])
     W = int(dims[1])
     H = int(dims[2])    
     file_out.write(dimensions + "\n" ) # L W H
     file_out.write(str( int(L*W*H*Dmax) ) + "\n" ) # Wmax
     nb_types = int(file.readline())
     file_out.write (str(nb_types)+"\n")
     for t in range(0,nb_types):
       type = file.readline().rstrip() # id l rl w rw h rh nb
       type_list = type.split()
       vol = int(type_list[1])*int(type_list[3])*int(type_list[5])
       type_list[-1] =  str(2*int(type_list[-1]))
       
       
       #file_out.write (type + " " + str(int(np.random.normal(mu, sigma)))+"\n")
       file_out.write (' '.join(type_list) + " " + str(int(vol*sample_trunc(mu, sigma, (0.0, np.inf)))) + " " + str(random.randint(1,101)) +"\n")
       #print(type)
       