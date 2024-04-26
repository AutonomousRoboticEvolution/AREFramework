cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

SET(VREP_SRC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/v_repLib.cpp>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/shared_memory.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/simLib.cpp>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/shared_memory.c>
    )

add_library(M_NIPES2 SHARED 
    ${VREP_SRC}
   factories.cpp
   M_NIPES.cpp 
   MNIPESLoggings.cpp
   ../common/obstacleAvoidance.cpp
   ../common/locomotion.cpp
   ../common/multiTargetMaze.cpp
   ../common/barrelTask.cpp
    )
target_include_directories(M_NIPES2 PUBLIC ${INCLUDES})
target_link_libraries(M_NIPES2 ARE simulatedER cmaes tbb)

install(TARGETS M_NIPES2 DESTINATION lib)
install(DIRECTORY . DESTINATION include/mnipes_v2 FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

