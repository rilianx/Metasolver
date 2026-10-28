Valparaíso, October 28, 2026

Editor
European Journal of Operational Research

Dear Editor,

Please find enclosed my manuscript "Fast exact overlap minimization for three-dimensional irregular strip
packing of polyhedra", which I submit for consideration for publication in the European Journal of Operational
Research.

The paper addresses the three-dimensional irregular strip packing problem, in which polyhedra, possibly
non-convex, are placed by translation in a container of fixed base so as to minimize its height. In two dimensions,
the best methods for irregular strip packing are overlap-minimization heuristics, but in three dimensions the
existing approaches either use an expensive exact overlap measure or approximate the pieces with voxels. The main
contribution is a fast exact move for polyhedra represented as unions of convex parts: the overlap of two parts is
measured by the penetration depth in their no-fit polytope, and the best position of a piece along an axis is
found exactly by evaluating a few breakpoints. Around this move, the paper builds a simple method that adapts known
components from two-dimensional nesting, and analyzes which of them matter.

On the classical benchmarks of polyhedral pieces, the method improves the best known heights of 11 of 13
instances within two minutes, reaches in 10 seconds the quality that the previous exact local search attains in
250 seconds (normalized to the same hardware), and matches 24 of the 27 proven optima of the small instances of
recent mixed-integer models. It also reports the limitations of the method on curved meshes, where voxel-based
approaches remain superior. The source code, the solutions and the scripts that reproduce all the experiments are
publicly available at https://github.com/rilianx/poly3d-packing.

I believe the paper fits the scope of the journal, which has published most of the recent work on irregular
packing, including the references compared with. The manuscript has not been published and is not under
consideration elsewhere. There are no conflicts of interest to declare.

Sincerely,

Ignacio Araya
Escuela de Ingeniería Informática, Pontificia Universidad Católica de Valparaíso, Chile
ignacio.araya@pucv.cl
