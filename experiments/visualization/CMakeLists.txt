cmake_minimum_required(VERSION 3.0)

find_package(Torch REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

SET(VREP_SRC
    ${COPPELIASIM_FOLDER}/programming/include/simLib/simLib.cpp
    ${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c
    )


add_library(visualization SHARED 
    ${VREP_SRC}
    visualization.cpp
    factories.cpp
    ../common/obstacleAvoidance.cpp
    ../common/barrelTask.cpp
    ../common/exploration.cpp
    ../common/locomotion.cpp
    ../common/hill_climbing.cpp
    ../common/push_object.cpp
    )
target_include_directories(visualization PUBLIC
   ${COPPELIASIM_INCLUDE_DIR}
   "../../are_torch/include"
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules"
    )
target_link_libraries(visualization ARE simulatedER  are_torch "${TORCH_LIBRARIES}")

install(TARGETS visualization DESTINATION lib/ARE/experiments/)
install(DIRECTORY . DESTINATION include/ARE/experiments/visualization  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
