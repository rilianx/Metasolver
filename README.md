# BSG for CLP

BSG is a method based on beam search(an adaptation of a branch-and-bound algorithm which expands only the most promising nodes at each level of the search tree). BSG explores the space to find a path from an it initial state (i.e., an empty container) to some complete solution or it terminal state (i.e., a loading plan in which no more boxes can be loaded).

## Compiling

```sh
git clone https://github.com/rilianx/Metasolver.git
make
```

All the build files will be in the newly created directory `build`.

## `BSG_CLP` Options

```plain
./build/BSG_CLP {OPTIONS} [instance-set]

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
      --show_layout                     Show the layout of the boxes
      --fsb                             full-support blocks
      --trace                           Trace
      instance-set                      The name of the instance set
      "--" can be used to terminate flag options and force all following
      arguments to be treated as positional options

    BSG Solver for CLP.
```

### `BSG_CLP` Example

```sh
./build/BSG_CLP problems/clp/benchs/BR/BR2.txt -i 47 -t 10
```

## BSG-B for biobjective CLP

BSG-B is an adaptation of the BSG algorithm for dealing with biobjective CLPs. It has two important differences w.r.t. BSG:

* BSG-B uses a dynamic mechanism for orienting the search to one objective or the other by changing the configuration of the heuristic function;
* BSG-B selects the states for the next level by using well-known multi-objective criteria, i.e., non-dominated sort (nds) and crowding distance;

| ![flowchart](https://i.ibb.co/TcWD4G2/BSG-BO-flowchart.png) |
| :---: |
| Flowchart of the BSG-B algorithm |

### The VPD function

VPD is an heuristic function for ranking and selecting the next block to be placed. VPD, like other heuristic functions, attempts to best evaluate those blocks that most probably will guide the construction to solutions maximizing the total volume of boxes.

The function VPD evaluates each block b according to the place in where it should be located inside the container. The mathematical expression of the VPD function is:

$$ VPD(b, \theta) = V(b)^{\theta_1} \cdot P(b)^{\theta_2} \cdot D(b)^{\theta_3} \cdot CS(b, \theta_4)^{\theta_5} \cdot (1 - L(b))^{\theta_6} \cdot N(b)^{-\theta_7} $$

where $V(b)$, $P(b)$ and $D(b)$ are the volume, profit and density of the block b respectively, $CS(b,\theta_4)$ is the proportion of the surface area of the block covered by the adjacent blocks or by the walls of the container. $L(b)$ estimates the wasted volume in the residual space of the selected free space cuboid. The estimation takes into account that the maximum usable space in each direction of the residual space must be a linear combination of the dimensions of the remaining boxes. The problem is modeled as a knapsack problem and solved with a standard algorithm in pseudo-polynomial time.

$\theta$ is the parameter vector of the function used for weighting the different criteria.

### `BSG_B` Options

```plain
./build/BSG_B {OPTIONS} [instance-set]

  ********* BSG-BO *********.

OPTIONS:

-h, --help                        Display this help menu
-i[int]                           Instance number
-f[string]                        Format (BR, BRw, 1C, BRwp)
--fp=[string]                     Kind of profit (R, I, W)
--min_fr=[double]                 Block_generation: Minimum volume occupied by a block (proportion)
--maxb=[int]                      Block_generation: Maximum number of generated blocks
-t[int], --timelimit=[int]        Timelimit
--seed=[int]                      Random seed
-s[double]                        Strategy (bsg, bsg_p, bsg_vp, ns+cd,
                                  o-search)
--theta_v=[double]                VPD's parameter vector (max vol)
--theta_p=[double]                VPD's parameter vector (max profit)
--ref=["double double"]           Reference point. (format: "y1 y2")
--matlab                          Matlab output
--trace                           Trace
instance-set                      The instance set

  BSG-B Solver for CLP.

```

### `BSG_B` Example

```sh
./build/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR8.txt -i 1 --min_fr=0.98 -t 10 --theta_v="1 0 -0.13 0.02 3.85 6.27 0.48" --theta_p="-0.95 1.0 -0.16 0.02 1.53 1.53 0.44" -s o-search --fp=R -f BRwp
```

## Instance sets

Set of benchmark instances can be found in [problems/clp/benchs](https://github.com/rilianx/Metasolver/tree/mop-bsg/problems/clp/benchs)

There are 3 sets of biobjective instances:

* [BRwp-X-Z](https://github.com/rilianx/Metasolver/tree/mop-bsg/problems/clp/benchs) (option -fp BRwp): 12 set of 1600 biobjective instances each. They are based on the BR instances proposed by [Bischoff & Ratcliff](https://www.sciencedirect.com/science/article/pii/030504839500015G) and [Davies & Bischoff](https://www.sciencedirect.com/science/article/abs/pii/S0377221798001398). We consider random weights ($w_i = v_i * d_i$) to each box $c_i$ of the BR instances. $d_i$ corresponds to the density of the box and it is randomly generated according to a normal distribution $N(\mu=X \cdot D_{max}, \sigma=X \cdot Z)$. We considered three different values for $X$ (0.5, 1.0 and 1.4) and three values for $Z$ (0.3, 0.5 and 0.8). $D_{max} = 753 \ [kg/m^3]$ corresponds to the maximum density supported by 20' containers. Profits of boxes were generated random and uniformly with values between $0$ and $100$. The three kinds of profits are supported: `--fp=R` for random profiles retrieved from the instance file, `--fp=W` for profits equal to weights of boxes and `--fp=I` for profits equal to $1$.

* [NMFTA](https://github.com/rilianx/Metasolver/tree/mop-bsg/problems/clp/benchs/NMFTA) (option `-f BRwp`): 11 sets of 100 instances generated by using directly real data provided by NMFTA in  [February,  2019](http://www.nmfta.org/pages/Public-Docket-Files-2019-1) (spreadsheest of data can be found [here](https://github.com/rilianx/Metasolver/tree/mop-bsg/extras/nmfta_generator)). Each instance was generated by selecting features of real transportation products related to a speific kind of commodity (e.g., mattresses, box spring or waterbeds; flammable or combustible liquids; power pumps; etc.). The three kinds of profits are supported: `--fp=R` for random profiles retrieved from the instance file, `--fp=W` for profits equal to weights of boxes  of boxes and `--fp=I` for profits equal to $1$.

* [1C](https://github.com/rilianx/Metasolver/tree/master/problems/clp/benchs/1C) (option `-f BRwp`): Sets of biobjective instances proposed by [Gonzalez et al.](https://www.sciencedirect.com/science/article/pii/S1877050916319494). Two kinds of profits are supported: `--fp=W` for profits equal to weights of boxes and `--fp=I` for profits equal to $1$.

## Variants

Five variants are implemented:

* **BSG** (option: `-s bsg`): corresponds to the original algorithm. Its objective is to maximize the occupied volume of the container, thus it selects as successors the nodes which, after applying the greedy algorithm, maximize the occupied volume. BSG uses the parameter vector $\theta_v$ for the heuristic function VPD.

* **BSG_p** (option: `-s bsg_p`): is equivalent to BSG but it selects as successors the nodes which, after applying the greedy algorithm, maximize the total profit of the loaded boxes. `BSG_p` uses the parameter vector $\theta_p$ for the function VPD.

* **BSG_vp** (option: `-s bsg_vp`):  runs `BSG` and `BSG_p` in sequence. Each strategy is run half of the total assigned time with parameter vectors $\theta_v$ and $\theta_p$ respectively.

* **nds+cd** (option: `-s nds+cd`) and **o-search** (option: `-s o-search`). Both use non-dominated sort and crowding distance for selecting successors in the beam search. In addition, o-search orients the search by changing dynamically the parameter values of the heuristic function VPD between $\theta_v$ and $\theta_p$.
nds+cd uses the parameter vector $\theta_v$ for the function VPD.

By default the values of $\theta_v$ and $\theta_p$ are tuned for maximizing the total volume and the total profit of loaded boxes respectively (i.e., `--theta_v="1 0 -0.13 0.02 3.85 6.27 0.48"` and `--theta_p="-0.95 1.0 -0.16 0.02 1.53 1.53 0.44"`).

## Experiments

* [Results of BSG on the classic BR instances.](https://docs.google.com/spreadsheets/u/1/d/e/2PACX-1vTp6t3tOLYOzoEXScyt5GEJxhu7oML3eq2yyX0J_t6YiMrTTf6XsXhkSsqJ5AxGkFjYBfUyl_vlLsAe/pubhtml?gid=1713343267&single=true)
* [Results of experiments on BRwp1.0-0.5 instances.](https://docs.google.com/spreadsheets/d/e/2PACX-1vSk0_iexrd_v7x-I_72Albt21t9iLka3o9CUatUYh0jcxl7O-Lw_dLpJ9FRF0GdlqQ7AyrCFXoCF5wX/pubhtml?gid=1240292960&single=true)
* [Results of experiments on NMFTA instances.](https://docs.google.com/spreadsheets/d/e/2PACX-1vSk0_iexrd_v7x-I_72Albt21t9iLka3o9CUatUYh0jcxl7O-Lw_dLpJ9FRF0GdlqQ7AyrCFXoCF5wX/pubhtml?gid=157777762&single=true)

## Related papers

* I. Araya, M.-C. Riff, [A beam search approach to the container loading problem](https://www.sciencedirect.com/science/article/pii/S0305054813002530), Computers & Operations Research 43 (2014) 100–107.
* I.  Araya,  K.  Guerrero,  E.  Núnez,  [VCS:  A  new  heuristic  function  for selecting boxes in the single container loading problem](https://www.sciencedirect.com/science/article/pii/S0305054817300023),  Computers & Operations Research 82 (2017) 27–35
* I.  Araya, M. Moyano, C. Sanchez, [A beam search algorithm for the biobjective container loading problem](https://www.sciencedirect.com/science/article/pii/S037722172030254X). European Journal of Operational Research, 286(2), 417-431. (2020)
