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
  problems/clp/clpState.cpp
  problems/clp/VCS_Function.cpp
)

# problema de carga de poliedros (PLP)
add_library(plp
  problems/plp/ConvexPolytope.cpp
  problems/plp/Json.cpp
  problems/plp/Mesh.cpp
  problems/plp/Instance.cpp
  problems/plp/Solution.cpp
)
target_include_directories(plp PUBLIC problems/plp)

add_executable(PLP_check problems/plp/main_plp_check.cpp)
target_include_directories(PLP_check PRIVATE problems/clp)
target_link_libraries(PLP_check plp)

add_executable(test_plp problems/plp/test_plp.cpp)
target_link_libraries(test_plp plp)

enable_testing()
add_test(NAME br7 COMMAND BSG_CLP ${CMAKE_CURRENT_SOURCE_DIR}/problems/clp/benchs/BR/BR7.txt -i 0 -t 2)
add_test(NAME plp_unit COMMAND test_plp ${CMAKE_CURRENT_SOURCE_DIR}/problems/plp/benchs)
add_test(NAME plp_check_stoyan2005 COMMAND PLP_check -q "${CMAKE_CURRENT_SOURCE_DIR}/problems/plp/benchs/Stoyan et al. 2005/Example1.json"
  -s "${CMAKE_CURRENT_SOURCE_DIR}/problems/plp/benchs/Stoyan et al. 2005/Solution1.json")
