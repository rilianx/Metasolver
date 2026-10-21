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

None of them changes the search: for the same amount of work, the solutions
found are the same as in the original implementation.

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
      -t[int], --timelimit=[int]        Timelimit
      --alpha=[double]                  Alpha parameter
      --beta=[double]                   Beta parameter
      --gamma=[double]                  Gamma parameter
      --delta=[double]                  Delta parameter
      -p[double]                        p parameter
      instance-set                      The name of the instance set (BR format)
```

Example
-----
     ./build/BSG_CLP problems/clp/benchs/BR/BR2.txt -i 47 -t 10
