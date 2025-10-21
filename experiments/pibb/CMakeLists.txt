cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

set(INCLUDES
   ${COPPELIASIM_INCLUDE_DIR}
   "../../are_torch/include"
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

SET(VREP_SRC
    ${COPPELIASIM_FOLDER}/programming/include/simLib/simLib.cpp
    ${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c
    )


add_library(pibb SHARED
    ${VREP_SRC}
    factories.cpp
    pibb_loggings.cpp
    pibb.cpp
    ../common/obstacleAvoidance.cpp
    ../common/barrelTask.cpp
    ../common/exploration.cpp
    #../common/multiTargetMaze.cpp
    ../common/locomotion.cpp
    ../common/hill_climbing.cpp
    ../common/push_object.cpp
    ../common/obstacleAvoidance.cpp
    )
target_include_directories(pibb PUBLIC ${INCLUDES})
target_link_libraries(pibb ARE
    simulatedER cmaes ${TORCH_LIBRARIES} are_torch)



install(TARGETS pibb DESTINATION lib/ARE/experiments/)
install(DIRECTORY . DESTINATION include/ARE/experiments/pibb FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

