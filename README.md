[![Build Status](https://travis-ci.org/rilianx/Metasolver.svg?branch=mop-bsg)](https://travis-ci.org/rilianx/Metasolver)
# Metasolver

## MCTS

Description
------



Installation
-------- 

Clone the project:
     
     git clone https://github.com/rilianx/Metasolver.git
     git checkout -t origin/mcts3

Compile the project:
     
     cmake .
then
     
     make MCTS_CLP


## BSG for CLP
Description
------
BSG is a method based on beam search(an adaptation of a branch-and-bound 
algorithm which expands only the most promising nodes at each level of the search tree).
BSG explores the space to find a path from an it initial state (i.e., an empty container) 
to some complete solution or it terminal state
(i.e., a loading plan in which no more boxes can be loaded).
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

## BSG-BO for biobjective CLP

Description
------
BSG-BO is an adaptation of the BSG algorithm for dealing with
biobjective CLPs. The algorithm has two important
differences w.r.t. BSG:
first, states for the next level of the tree
are selected using well-known
multi-objective criteria
(non-dominated sort and crowding distance);
second, the selection of the next blocks in both,
the get-expanded-nodes procedure
and the greedy algorithm, is performed
by using a modification of the VCS function
which takes into account the second objective.

```
./BSG_BO_CLP {OPTIONS} [instance-set]

  ********* BSG-BO *********.

OPTIONS:

    -h, --help                        Display this help menu
    -i[int]                           Instance
    -f[string]                        Format: (BR, BRw, 1C)
    --min_fr=[double]                 Minimum volume occupied by a block
                                      (proportion)
    --maxb=[int]                      Maximum number ob generated blocks
    -t[int], --timelimit=[int]        Timelimit
    --seed=[int]                      Random seed
    --alpha=[double]                  Alpha parameter in the VCS function
    --beta=[double]                   Beta parameter in the VCS function
    --gamma=[double]                  Gamma parameter in the VCS function
    --delta=[double]                  Delta parameter in the VCS function
    -p[double]                        p parameter
    -c[double], --maxtheta=[double]   ponderation of the density in the VCS
                                      function
    --theta=[double]                  Weight of the second objective in the
                                      greedy
    --srule=[double]                  BSG-BO selection rule (NSGA2, MIN1,
                                      MIN2)
    --fsb                             full-support blocks
    --trace                           Trace
    instance-set                      The name of the instance set
    "--" can be used to terminate flag options and force all following
    arguments to be treated as positional options

  BSG-BO Solver for CLP.

```

Examples
-----
     ./BSG_BO_CLP problems/clp/benchs/BR/BR2.txt -i 47 --min_fr=1.0 -t 30 -f BR --alpha=3.0 --beta=2.0 --gamma=0.5 -p 0.04 -c 0.5
     ./BSG_BO_CLP problems/clp/benchs/BRrw/BRrw9.txt -i 12 --min_fr=0.98 -t 30 -f BRw --alpha=3.0 --beta=2.0 --gamma=0.5 -p 0.04 -c 0.5

Instance sets
-----
Set of benchmark instances can be found in problems/clp/benchs/

There are 4 sets of instances:

* [BR](https://github.com/rilianx/Metasolver/tree/master/problems/clp/benchs/BR): 16 set of 100 single-objective instances each. BR0, BR8-BR15 were proposed by [Bischoff & Ratcliff](https://www.sciencedirect.com/science/article/pii/030504839500015G) while
BR1-BR7 were proposed by [Davies & Bischoff](https://www.sciencedirect.com/science/article/abs/pii/S0377221798001398).
Weights are not included (by default, BSG_BO_CLP consider a weight=1 for each box).
* [BRrw](https://github.com/rilianx/Metasolver/tree/master/problems/clp/benchs/BRrw): BR instances with random generated weights. Weights of box types were randomly generated
by sampling from an uniform distribution between 0 and 1.
* [BRrd](https://github.com/rilianx/Metasolver/tree/master/problems/clp/benchs/BRrd): BR instances with random generated weights. First, density of box types were
randomly generated by sampling from an uniform distribution between 0.8 and 1.2.
Then, the weights are get: weight <-- density*volume
* [1C](https://github.com/rilianx/Metasolver/tree/master/problems/clp/benchs/1C): Sets of biobjective instances proposed by [Gonzalez et al.](https://www.sciencedirect.com/science/article/pii/S1877050916319494).
