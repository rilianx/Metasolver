# HelloWorld is a minimal sample creating, stepping and deleting a Bullet dynamics world

cmake_minimum_required(VERSION 3.1)
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
problems/mclp
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


 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -no-pie")

# ADD_DEFINITION(-g -pg)

ADD_LIBRARY(
bsg
metasolver/SearchStrategy.cpp
metasolver/GlobalVariables.cpp
metasolver/strategies/BSG.cpp
metasolver/strategies/Greedy.cpp
metasolver/State.cpp
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
)

ADD_LIBRARY(
clp_state
problems/clp/clpState.cpp
)

ADD_LIBRARY(
mclp_state
problems/mclp/mclp-state.cpp
)

ADD_LIBRARY(
clp_kdtree
problems/clp/plugins/kdtree/clpStatekd.cpp
problems/clp/plugins/kdtree/kdNode.cpp
problems/clp/plugins/kdtree/BlockSet.cpp
)


ADD_EXECUTABLE(MCLP problems/mclp/main/mclp-solver.cpp)
ADD_EXECUTABLE(BSG_CLP problems/clp/main_clp.cpp)
ADD_EXECUTABLE(BSG_CLP_ANN problems/clp/plugins/ann/main_clp_ann.cpp)
ADD_EXECUTABLE(CLP_GENSOLV problems/clp/plugins/ann/gen_ran_inst_solve.cpp)
ADD_EXECUTABLE(test_BSG_CLP problems/clp/test_clp.cpp)
ADD_EXECUTABLE(BSG_BO_CLP problems/clp/clp_bsg-mop.cpp)
ADD_EXECUTABLE(test_BSGMOP_CLP problems/clp/test_clp_mop.cpp)
ADD_EXECUTABLE(testEvalMOP problems/clp/test_evalMOP.cpp)
ADD_EXECUTABLE(CLP_gen problems/clp/ClpInstanceGenerator.cpp)
ADD_EXECUTABLE(NSGA_CLP problems/clp/plugins/ga/main.cpp)

target_link_libraries(MCLP mclp_state  clp bsg)
target_link_libraries(BSG_CLP clp_state clp clp_kdtree bsg)
target_link_libraries(BSG_CLP_ANN clp_state clp clp_kdtree bsg)
target_link_libraries(test_BSG_CLP clp_state clp clp_kdtree bsg)
target_link_libraries(CLP_GENSOLV clp_state clp clp_kdtree bsg)
target_link_libraries(NSGA_CLP clp_state clp nsga2 bsg)

target_link_libraries(BSG_BO_CLP bsg_mop clp_state clp bsg)
target_link_libraries(test_BSGMOP_CLP bsg_mop clp_state clp bsg)
target_link_libraries(testEvalMOP bsg_mop clp_state clp bsg)

enable_testing()

add_test( ./BSG_CLP problems/clp/benchs/BR/BR5.txt -t 5 )



IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  DEBUG_POSTFIX "_Debug")
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
