import subprocess
import random
import sys

# From the root directory:
#  python extras/generate_trainingdata.py maxtime nb_instances output_file

maxtime = sys.argv[1]
nb_instances = int(sys.argv[2])
output_file = sys.argv[3]



for i in range(1,nb_instances):
    N=random.randint(1,101)
    seed=random.randint(1000,100000)
    minvol=random.randint(0,101)*0.01;

    args = ["./CLP_GENSOLV", "--solve", "--maxtime", maxtime ,"-N", str(N) , "--min_vol", str(minvol) ,"-s", str(seed) ,"-r" , "0.5"]
    proc = subprocess.Popen(args, stdout=subprocess.PIPE)

    print(str(i)+ ":"+'[%s]' % ', '.join(map(str, args)))

    output = proc.communicate()[0]
    f = open(output_file, "a")
    f.write(str(i)+":"+'[%s]' % ', '.join(map(str, args))+"\n")
    f.write(output+"\n")
    f.close()
