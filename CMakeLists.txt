# HelloWorld is a minimal sample creating, stepping and deleting a Bullet dynamics world

cmake_minimum_required(VERSION 3.5)


set (CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES(
lib/bullet-2.80/src
metasolver
metasolver/strategies/
problems/clp/containers
problems/clp/objects2
problems/clp/evaluators
problems/clp
problems/clp/plugins/kdtree
problems/clp/plugins/ga
problems/cpmp
metasolver/plugins/mop/strategies/
)

LINK_DIRECTORIES(
lib/bullet-2.80/src/BulletCollision
lib/bullet-2.80/src/BulletDynamics
lib/bullet-2.80/src/LinearMath
)

LINK_LIBRARIES(
 BulletDynamics BulletCollision LinearMath
)
    if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
    endif(COMMAND cmake_policy)


 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# ADD_DEFINITION(-g -pg)

ADD_LIBRARY(
base
metasolver/SearchStrategy.cpp
metasolver/GlobalVariables.cpp
)

ADD_LIBRARY(
bsg
metasolver/strategies/BSG.cpp
metasolver/strategies/Greedy.cpp
)

ADD_LIBRARY(
a_star
metasolver/strategies/A_star.cpp
)

ADD_LIBRARY(
bsg_mop
 metasolver/plugins/mop/strategies/BSGMOP.cpp
)

ADD_LIBRARY(
nsga2
 metasolver/plugins/mop/strategies/Chromosome.cpp
 metasolver/plugins/mop/strategies/NSGA2.cpp
 problems/clp/plugins/ga/clp_chromosome.cpp
)

ADD_LIBRARY(
clp
problems/clp/containers/AABBContainer.cpp
problems/clp/containers/AABBTree.cpp
problems/clp/evaluators/VCS_Function.cpp
problems/clp/evaluators/VLossFunction.cpp
problems/clp/objects2/AABB.cpp
problems/clp/objects2/Block.cpp
problems/clp/objects2/BoxShape.cpp
problems/clp/objects2/Space.cpp
problems/clp/objects2/SpaceSet.cpp
problems/clp/objects2/Block_fsb.cpp
problems/clp/clpState.cpp
)

ADD_LIBRARY(
cpmp
problems/cpmp/cpmpState.cpp
)

ADD_LIBRARY(
clp_kdtree
problems/clp/plugins/kdtree/clpStatekd.cpp
problems/clp/plugins/kdtree/kdNode.cpp
problems/clp/plugins/kdtree/BlockSet.cpp
)

ADD_EXECUTABLE(BSG_CLP problems/clp/main_clp.cpp)
ADD_EXECUTABLE(BSG_CLP_ANN problems/clp/plugins/ann/main_clp_ann.cpp)
ADD_EXECUTABLE(CLP_GENSOLV problems/clp/plugins/ann/gen_ran_inst_solve.cpp)
ADD_EXECUTABLE(test_BSG_CLP problems/clp/test_clp.cpp)
ADD_EXECUTABLE(BSGMOP_CLP problems/clp/clp_bsg-mop.cpp)
ADD_EXECUTABLE(test_BSGMOP_CLP problems/clp/test_clp_mop.cpp)
ADD_EXECUTABLE(testEvalMOP problems/clp/test_evalMOP.cpp)
ADD_EXECUTABLE(CLP_gen problems/clp/ClpInstanceGenerator.cpp)
ADD_EXECUTABLE(NSGA_CLP problems/clp/plugins/ga/main.cpp)
ADD_EXECUTABLE(A_CPMP problems/cpmp/cpmp_main.cpp)

target_link_libraries(BSG_CLP bsg clp clp_kdtree base)
target_link_libraries(BSG_CLP_ANN bsg clp clp_kdtree base)
target_link_libraries(test_BSG_CLP bsg clp clp_kdtree base)
target_link_libraries(CLP_GENSOLV bsg clp clp_kdtree base)
target_link_libraries(NSGA_CLP bsg clp nsga2 base)
target_link_libraries(A_CPMP a_star cpmp base)

target_link_libraries(BSGMOP_CLP bsg_mop bsg clp base)
target_link_libraries(test_BSGMOP_CLP bsg_mop bsg clp base)
target_link_libraries(testEvalMOP bsg_mop bsg clp base)

enable_testing()

add_test( test_BSG_CLP test_BSG_CLP problems/clp/benchs/BR/BR8.txt 1 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0 )
add_test( test_BSGMOP_CLP test_BSGMOP_CLP problems/clp/benchs/BR/BR6.txt 1 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0 0 )





IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  DEBUG_POSTFIX "_Debug")
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
