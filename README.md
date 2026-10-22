# BSG for CLP

Description
------
BSG is a method based on beam search (an adaptation of a branch-and-bound
algorithm which expands only the most promising nodes at each level of the search tree).
BSG explores the space to find a path from an initial state (i.e., an empty container)
to some complete solution or terminal state
(i.e., a loading plan in which no more boxes can be loaded).

This branch (`bsg-clp`) contains only the single-objective BSG for the
Container Loading Problem with instances in BR format
(Bischoff & Ratcliff / Davies & Bischoff). The multi-objective variants
(BSG-B, NSGA-II), other problems and plugins are available in `master`.

Code structure
-------
* `metasolver/`: generic search framework (`State`, `SearchStrategy`) and
  strategies: `Greedy`, `BSG` (beam search guided by greedy solutions) and
  `DoubleEffort` (runs BSG doubling the effort until the time limit).
* `problems/clp/`: the CLP
  * `clpState`: partial solution (container, remaining boxes and valid blocks),
    instance reading (`new_state`).
  * `Block`: blocks of boxes and guillotine block generation.
  * `Container`: placed blocks and maximal free spaces.
  * `VCS_Function`: VCS evaluation function (volume, pseudo-contact surface,
    volume loss and number of boxes).
* `problems/plp/`: packing of polyhedra (3D irregular strip packing, ESICUP
  instances): instance and solution readers and a solution checker
  (`PLP_check`). See `problems/plp/README.md`.

Implementation notes
-------
The implementation follows the code optimizations proposed by L. Guesser
in his dissertation (chapter 4):
* free spaces and placed blocks are stored in contiguous vectors
  (no external collision-detection library);
* the pseudo-contact surface is computed by first filtering the placed blocks
  near the selected free space and then evaluating each candidate block;
* blocks are deduplicated with a hash during the block generation;
* the greedy remembers the states already visited (skip feature) and
  rebuilds the rest of the solution instead of evaluating it again.

Besides, the volume loss of a candidate block is only computed when an upper
bound of its evaluation can enter the n best ones, and the blocks of the
instance are stored contiguously in memory.

None of them changes the search: for the same parameters, the solutions found
are the same as in the original implementation (~9x faster).

The default parameters of the evaluation function are the ones tuned by
L. Guesser with irace (alpha=2.4407, beta=2.2652, gamma=0.3896, p=0.0164).
On the BR instances (sets 1-15, 10 instances per set, 30 s) they improve
the volume utilization by ~0.17 percentage points w.r.t. the parameters of
Araya et al. (2017): alpha=4, beta=1, gamma=0.2, p=0.04. With these parameters
the search is the same as in the original implementation.

Installation
-------

     git clone -b bsg-clp https://github.com/rilianx/Metasolver.git
     cd Metasolver
     mkdir build && cd build
     cmake ..
     make

Options
--------

```
./BSG_CLP {OPTIONS} [instance-set]

  OPTIONS:

      -h, --help                        Display this help menu
      -i[int]                           Instance
      --min_fr=[double]                 Minimum volume occupied by a block
                                        (proportion)
      -t[int], --timelimit=[int]        Timelimit (it includes the block generation)
      --alpha=[double]                  Alpha parameter. Default: 2.4407
      --beta=[double]                   Beta parameter. Default: 2.2652
      --gamma=[double]                  Gamma parameter. Default: 0.3896
      --delta=[double]                  Delta parameter
      -p[double]                        p parameter. Default: 0.0164
      --fix_knapsack                    Volume loss: consider the lengths of all
                                        the allowed orientations of the boxes
      instance-set                      The name of the instance set (BR format)
```

`--fix_knapsack`: the knapsack problems used to estimate the volume loss only
consider the orientations LWH, WHL and HLW of each box, which omits valid
lengths in the x and y axes when the box cannot be placed with its length in
vertical. With this option all the allowed orientations are considered (it
does not change the average results significantly).

Example
-----
     ./build/BSG_CLP problems/clp/benchs/BR/BR2.txt -i 47 -t 10
