[![Build Status](https://travis-ci.org/rilianx/Metasolver.svg?branch=mop-bsg)](https://travis-ci.org/rilianx/Metasolver)
# Metasolver


Clonar con comando el comando
git clone https://github.com/rilianx/Metasolver.git

Compilar el proyecto con el comando
cmake .

## BSG_CLP

```
./BSG_CLP {OPTIONS} [instance-set]

    ********* BSG-CLP *********.

  OPTIONS:

      -h, --help                        Display this help menu
      -i[int]                           Instance
      -f[string]                        Format: (BR, BRw, 1C)
      --min_fr=[double]                 Minimum volume occupied by a block
                                        (proportion)
      -t[int], --timelimit=[int]        Timelimit
      --seed=[int]                      Random seed
      --alpha=[double]                  Alpha parameter
      --beta=[double]                   Beta parameter
      --gamma=[double]                  Gamma parameter
      --delta=[double]                  Delta parameter
      -p[double]                        p parameter
      --fsb                             full-support blocks
      --trace                           Trace
      instance-set                      The name of the instance set
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options

    BSG Solver for CLP.
```

## BSGMOP_CLP

```
  ./BSGMOP_CLP {OPTIONS} [instance-set]

    ********* BSG-MOP CLP *********.

  OPTIONS:

      -h, --help                        Display this help menu
      -i[int]                           Instance
      -f[string]                        Format: (BR, BRw, 1C)
      --min_fr=[double]                 Minimum volume occupied by a block
                                        (proportion)
      --maxb=[int]                      Maximum number ob generated blocks
      -t[int], --timelimit=[int]        Timelimit
      --seed=[int]                      Random seed
      --alpha=[double]                  Alpha parameter
      --beta=[double]                   Beta parameter
      --gamma=[double]                  Gamma parameter
      --delta=[double]                  Delta parameter
      -p[double]                        p parameter
      --theta=[double]                  Weight of the second objective in the
                                        greedy
      --maxtheta=[double]               ponderation of the weight of a box for
                                        maximizing the total weight
      --srule=[double]                  BSGMOP selection rule (NSGA2, MIN1,
                                        MIN2)
      --fsb                             full-support blocks
      --trace                           Trace
      instance-set                      The name of the instance set
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options

    BSG-MOP Solver for CLP.
```


