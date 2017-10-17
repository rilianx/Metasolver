# HelloWorld is a minimal sample creating, stepping and deleting a Bullet dynamics world


set (CMAKE_CXX_STANDARD 11)

INCLUDE_DIRECTORIES(
lib/bullet-2.80/src
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
bsg
metasolver/GlobalVariables.cpp
metasolver/strategies/BSG.cpp
metasolver/strategies/Greedy.cpp  
)

ADD_LIBRARY(
bsg_mop
metasolver/GlobalVariables.cpp
metasolver/strategies/BSG.cpp
metasolver/strategies/Greedy.cpp  
metasolver/plugins/mop/strategies/BSGMOP.cpp
)

ADD_LIBRARY(
clp
clp/containers/AABBContainer.cpp 
clp/containers/AABBTree.cpp 
clp/evaluators/VCS_Function.cpp 
clp/evaluators/VLossFunction.cpp 
clp/objects2/AABB.cpp 
clp/objects2/Block.cpp 
clp/objects2/BoxShape.cpp 
clp/objects2/Space.cpp
clp/objects2/SpaceSet.cpp
clp/objects2/Block_fsb.cpp
clp/clpState.cpp 
)

ADD_LIBRARY(
clp_kdtree
clp/plugins/kdtree/clpStatekd.cpp 
clp/plugins/kdtree/kdNode.cpp 
clp/plugins/kdtree/BlockSet.cpp 
)


ADD_EXECUTABLE(CLP2017 main_clp.cpp)

ADD_EXECUTABLE(CLP_MOP test_clp_mop.cpp)

target_link_libraries(CLP2017 bsg clp clp_kdtree)

target_link_libraries(CLP_MOP bsg bsg_mop clp)
 
ENDIF()



IF (INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  DEBUG_POSTFIX "_Debug")
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  MINSIZEREL_POSTFIX "_MinsizeRel")
			SET_TARGET_PROPERTIES(AppHelloWorld PROPERTIES  RELWITHDEBINFO_POSTFIX "_RelWithDebugInfo")
ENDIF(INTERNAL_ADD_POSTFIX_EXECUTABLE_NAMES)
