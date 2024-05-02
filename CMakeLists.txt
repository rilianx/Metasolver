CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project(MetaSolver)

# Prevent building in the source directory

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.\n")
endif()

# Build options

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Dependencies

## Bullet
set(BULLET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet-2.80/src )
set(BULLET_STATIC_LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/static/)
find_library(
    BULLET_DYNAMICS_LIBRARY
    NAME BulletDynamics
    PATHS ${BULLET_STATIC_LIBRARIES_DIR}
    REQUIRED
)
find_library(
    BULLET_COLLISION_LIBRARY
    NAME BulletCollision
    PATHS ${BULLET_STATIC_LIBRARIES_DIR}
    REQUIRED
)
find_library(
    BULLET_LINEAR_MATH_LIBRARY
    NAME LinearMath
    PATHS ${BULLET_STATIC_LIBRARIES_DIR}
    REQUIRED
)
add_library(Bullet STATIC IMPORTED)
set_target_properties(Bullet PROPERTIES IMPORTED_LOCATION ${BULLET_COLLISION_LIBRARY})
target_include_directories(Bullet SYSTEM INTERFACE ${BULLET_INCLUDE_DIR})
target_link_libraries(Bullet INTERFACE ${BULLET_DYNAMICS_LIBRARY} ${BULLET_COLLISION_LIBRARY} ${BULLET_LINEAR_MATH_LIBRARY})

link_libraries(Bullet)

include_directories(
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

add_library(
    bsg
    metasolver/SearchStrategy.cpp
    metasolver/GlobalVariables.cpp
    metasolver/strategies/BSG.cpp
    metasolver/strategies/Greedy.cpp
    metasolver/State.cpp
)

add_library(
    bsg_mop
    metasolver/plugins/mop/strategies/BSGMOP.cpp
)

add_library(
    nsga2
    metasolver/plugins/mop/strategies/Chromosome.cpp
    metasolver/plugins/mop/strategies/NSGA2.cpp
    problems/clp/plugins/ga/clp_chromosome.cpp
)

add_library(
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

add_library(
    clp_state
    problems/clp/clpState.cpp
)

add_library(
    mclp_state
    problems/mclp/mclp-state.cpp
)

add_library(
    clp_kdtree
    problems/clp/plugins/kdtree/clpStatekd.cpp
    problems/clp/plugins/kdtree/kdNode.cpp
    problems/clp/plugins/kdtree/BlockSet.cpp
)

# Executables

add_executable(MCLP problems/mclp/main/mclp-solver.cpp)
target_link_libraries(MCLP mclp_state  clp bsg)

add_executable(BSG_CLP problems/clp/main_clp.cpp)
target_link_libraries(BSG_CLP clp_state clp clp_kdtree bsg)

add_executable(BSG_CLP_ANN problems/clp/plugins/ann/main_clp_ann.cpp)
target_link_libraries(BSG_CLP_ANN clp_state clp clp_kdtree bsg)

add_executable(CLP_GENSOLV problems/clp/plugins/ann/gen_ran_inst_solve.cpp)
target_link_libraries(CLP_GENSOLV clp_state clp clp_kdtree bsg)

add_executable(BSG_B problems/clp/clp_bsg-mop.cpp)
target_link_libraries(BSG_B bsg_mop clp_state clp bsg)

add_executable(CLP_gen problems/clp/ClpInstanceGenerator.cpp)

add_executable(NSGA_CLP problems/clp/plugins/ga/main.cpp)
target_link_libraries(NSGA_CLP clp_state clp nsga2 bsg)

# Tests

enable_testing()

add_executable(test_BSG_CLP problems/clp/test_clp.cpp)
target_link_libraries(test_BSG_CLP clp_state clp clp_kdtree bsg)

add_executable(test_BSGMOP_CLP problems/clp/test_clp_mop.cpp)
target_link_libraries(test_BSGMOP_CLP bsg_mop clp_state clp bsg)

add_test(
    NAME o-search
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 1 -s o-search --fp=R -f BRwp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME bsg
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 1 -s bsg --fp=R -f BRwp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME bsg_p
     COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 1 -s bsg_p --fp=R -f BRwp
     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_test(
    NAME bsg_vp
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/BSG_B problems/clp/benchs/BRwp-1.0-0.5/BR4.txt -i 1 --min_fr=0.98 -t 2 -s bsg_vp --fp=R -f BRwp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
