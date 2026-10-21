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

add_executable(BSG_CLP
  problems/clp/main_clp.cpp
  metasolver/SearchStrategy.cpp
  metasolver/State.cpp
  metasolver/strategies/BSG.cpp
  metasolver/strategies/Greedy.cpp
  problems/clp/Block.cpp
  problems/clp/Container.cpp
  problems/clp/LNS.cpp
  problems/clp/CPSAT.cpp
  problems/clp/clpState.cpp
  problems/clp/VCS_Function.cpp
)

target_link_libraries(BSG_CLP ${CMAKE_DL_LIBS})

enable_testing()
add_test(NAME br7 COMMAND BSG_CLP ${CMAKE_CURRENT_SOURCE_DIR}/problems/clp/benchs/BR/BR7.txt -i 0 -t 2)
