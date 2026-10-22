cmake_minimum_required(VERSION 3.5)
project(BSG_CLP CXX)

set(CMAKE_CXX_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(
  metasolver
  metasolver/strategies
  problems/clp
)

set(CLP_SOURCES
  metasolver/SearchStrategy.cpp
  metasolver/State.cpp
  metasolver/strategies/BSG.cpp
  metasolver/strategies/Greedy.cpp
  problems/clp/Block.cpp
  problems/clp/Container.cpp
  problems/clp/clpState.cpp
  problems/clp/VCS_Function.cpp
  problems/clp/ULD.cpp
)

add_executable(BSG_CLP problems/clp/main_clp.cpp ${CLP_SOURCES})
add_executable(BSG_ULD problems/clp/main_uld.cpp ${CLP_SOURCES})

enable_testing()
add_test(NAME br7 COMMAND BSG_CLP ${CMAKE_CURRENT_SOURCE_DIR}/problems/clp/benchs/BR/BR7.txt -i 0 -t 2)
add_test(NAME uld50 COMMAND BSG_ULD ${CMAKE_CURRENT_SOURCE_DIR}/problems/clp/benchs/ULD/Final_instances/n50sample0.csv -t 0.5)
