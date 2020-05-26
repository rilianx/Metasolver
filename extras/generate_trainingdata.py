import subprocess
import random
import sys

# From the root directory:
#  python extras/generate_trainingdata.py maxtime nb_instances output_file

maxtime = sys.argv[1]
nb_instances = int(sys.argv[2])
output_file = sys.argv[3]


i=0
while i < nb_instances:
    i += 1
    N=random.randint(1,101)
    seed=random.randint(1000,100000)
    minvol=random.randint(0,80)*0.01;

#    args = ["./CLP_GENSOLV", "--solve", "--maxtime", maxtime ,"-N", str(N) , "--min_vol", str(minvol) ,"-s", str(seed) ,"-r" , "0.5"]
    args = ["./TRAINGEN_PROB2VCS", "--solve", "-N", str(N) , "-s", str(seed)]

    proc = subprocess.Popen(args, stdout=subprocess.PIPE)

    print(str(i)+ ":"+'[%s]' % ', '.join(map(str, args)))

    output = proc.communicate()[0]
    if not output:
       i -= 1
       continue;

    f = open(output_file, "a")
    f.write(str(i)+":"+'[%s]' % ', '.join(map(str, args))+"\n")
    f.write(output+"\n")
    f.close()
