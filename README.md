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

Air cargo loading (BSG_ULD)
-------
`BSG_ULD` solves the air cargo palletization problem with the instances of
Paquay et al. (2018) (`problems/clp/benchs/ULD`): all the boxes must be loaded
into unit load devices (ULDs) of six types (Boeing 777), minimizing the number of
ULDs. The ULDs are loaded one by one with BSG; the next ULD is selected as in
Hessler et al. (2024) and, at the end, the boxes of the last ULD are reloaded into
a smaller ULD when possible.

Constraints:
* ULD shapes: up to four corners cut in the x-z plane (a block is moved away from
  a cut when it does not fit at the anchor corner of the space);
* weight capacity of the ULD;
* allowed orientations of the boxes and fragile boxes (nothing on top of them);
* support (`--support`): blocks are generated with all their boxes fully supported.
  With `corners` (default, the criterion of Paquay et al. 2018 and Hessler et al. 2024),
  the four bottom corners of each box must rest on non-fragile boxes or on the floor and
  no box can touch the top of a fragile box. With `full`, the blocks are placed on the
  floor or on blocks whose top face is complete and without fragile boxes;
* center of gravity: at most the allowed deviation from the center in x and y. The
  load of each ULD is translated to center its center of gravity, the evaluation of the
  solutions is divided by 1 + p * (relative excess of the deviation), and if the deviation
  is exceeded, the ULD is loaded again anchoring all the spaces at the origin
  (the load is grouped and it can be centered).

Each ULD is loaded with a subset of the remaining boxes (`--subset_ratio`, default 2):
the largest boxes that fit in the ULD, up to twice the volume of the ULD (0: all the boxes).
The boxes that are not loaded return to the pool.

After the sequential loading, the program tries to eliminate ULDs (`--no_eliminate`
disables it): the boxes of a ULD (from the least utilized) are freed and the other ULDs
are loaded again with their boxes and the freed ones; the boxes that are not loaded pass
to the next ULD. The ULD is eliminated if all the freed boxes are loaded.

The solutions are verified box by box. With `-o file` the solution is written (one line
per box: ULD, ULD type, item (line of the instance file, from 0), position and dimensions)
and it can be checked with an independent validator:

     python3 problems/clp/validate_uld.py <instance.csv> problems/clp/benchs/ULD/uld.csv <solution> [--support corners|full|none]

Results on the 300 final instances (30 per size, 10 to 100 boxes), number of ULDs
and center of gravity violations (Paquay et al. 2018: 1010 ULDs, 12.9%; Hessler et al.
2024: 708 ULDs, 11.2%, <0.5 s per instance):

| -t (per ULD) | subset ratio | elimination | ULDs | CoG violations | avg time per instance |
|---|---|---|---|---|---|
| 1 s   | all | no  | 613 | 0.3% | 2.5 s  |
| 1 s   | all | yes | 611 | 0.0% | 5.5 s  |
| 1 s   | 2   | yes | 605 | 0.0% | 5.4 s  |
| 0.1 s | all | no  | 638 | 0.9% | 0.27 s |
| 0.1 s | all | yes | 636 | 0.2% | 0.61 s |
| 0.1 s | 2   | yes | 629 | 0.2% | 0.60 s |

```
./BSG_ULD {OPTIONS} [instance]

      -t[double]                        Timelimit of the search in each ULD
                                        (seconds, without the block generation). Default: 1
      --cog_penalty=[double]            Penalty p of the deviation of the center of gravity. Default: 10
      --support=[string]                corners, full or none. Default: corners
      --max_bl=[int]                    Maximum number of blocks. Default: 1000
      --subset_ratio=[double]           Volume of the boxes given to each ULD (ratio of its volume, 0: all). Default: 2
      --no_eliminate                    Do not try to eliminate ULDs
      --uld=[string]                    ULD file (default: uld.csv in the parent directory of the instance)
      -o[string], --output=[string]     Write the solution to this file
      -v, --verbose                     Print the loaded ULDs
```

Example:

     ./build/BSG_ULD problems/clp/benchs/ULD/Final_instances/n50sample0.csv -t 1 -v
