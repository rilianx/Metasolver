
CLP Instance Generator (and solver)
==

Generates a random instance and solves it.


**Ejemplo de uso:**

```
./CLP_GENSOLV -N nb_box_types -s seed --min_fr=1.0 --maxtime=500 --solve -f /home/ignacio/clp/Metasolver/problems/clp/plugins/ann/train3/
```

```
  OPTIONS:

      -h, --help                        Display this help menu
      -L[int]                           Length of the container
      -W[int]                           Width of the container
      -H[int]                           Height of the container
      -N[int]                           Number of box types
      --minl=[int]                      Minimum length
      --minw=[int]                      Minimum width
      --minh=[int]                      Minimum height
      --maxl=[int]                      Maximum length
      --maxw=[int]                      Maximum width
      --maxh=[int]                      Maximum height
      --bsl=[int]                       Box stability limit
      -s[int]                           Generator seed
      --min_fr=[double]                 Minimum volume of a block
      --alpha=[double]                  Parameter alpha of VCS (default value=4.0)
      --beta=[double]                   Parameter beta of VCS (default value=1.0)
      --gamma=[double]                  Parameter gamma of VCS (default value=0.2)
      -p[double]                        Parameter p of VCS (default value=0.04)
      --maxtime=[int]                   Max Time
      -f[string]                        The path of the work folder
      --solve                           Solve the generated instance
```

