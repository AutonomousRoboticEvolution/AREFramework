cmake_minimum_required(VERSION 3.0)


SET(COPPELIA_SRC
    ${COPPELIASIM_FOLDER}/programming/include/simLib/simLib.cpp
    ${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApi.c
    ${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApiPlatform.c
    ${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c
)

add_library(visualization2 SHARED 
    ${COPPELIA_SRC}
    visualization.cpp
    factories.cpp
    ../common/obstacleAvoidance.cpp
    ../common/exploration.cpp
    ../common/barrelTask.cpp
    ../common/locomotion.cpp
    ../common/hill_climbing.cpp
    ../common/push_object.cpp
    )
target_include_directories(visualization2 PUBLIC
    ${COPPELIASIM_INCLUDE_DIR}
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules"
    )
target_link_libraries(visualization2 ARE simulatedER)
install(TARGETS visualization2 DESTINATION lib/ARE/experiments/)
install(DIRECTORY . DESTINATION include/ARE/experiments/visualization2  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
