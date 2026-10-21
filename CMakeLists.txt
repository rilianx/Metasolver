cmake_minimum_required(VERSION 3.5)
project(BSG_CLP CXX)

set(CMAKE_CXX_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(
  lib/bullet-2.80/src
  metasolver
  metasolver/strategies
  problems/clp
  problems/clp/containers
  problems/clp/objects2
  problems/clp/evaluators
)

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet-2.80/src/BulletCollision
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/bullet-2.80/src/LinearMath
)

add_executable(BSG_CLP
  problems/clp/main_clp.cpp
  metasolver/SearchStrategy.cpp
  metasolver/State.cpp
  metasolver/strategies/BSG.cpp
  metasolver/strategies/Greedy.cpp
  problems/clp/clpState.cpp
  problems/clp/containers/AABBTree.cpp
  problems/clp/evaluators/VCS_Function.cpp
  problems/clp/evaluators/VLossFunction.cpp
  problems/clp/objects2/AABB.cpp
  problems/clp/objects2/Block.cpp
  problems/clp/objects2/BoxShape.cpp
  problems/clp/objects2/Space.cpp
  problems/clp/objects2/SpaceSet.cpp
)
target_link_libraries(BSG_CLP BulletCollision LinearMath)

enable_testing()
add_test(NAME br7 COMMAND BSG_CLP ${CMAKE_CURRENT_SOURCE_DIR}/problems/clp/benchs/BR/BR7.txt -i 0 -t 2)
