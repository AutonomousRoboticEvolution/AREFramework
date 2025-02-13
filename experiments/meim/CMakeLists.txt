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

add_library(meim SHARED
    ${VREP_SRC}
    ../common/homeokinesis_controller.cpp
    ../common/obstacleAvoidance.cpp
    factories.cpp
    meim.cpp
    meim_logging.cpp
    ../common/fixed_controller.cpp
  )
target_include_directories(meim PUBLIC ${INCLUDES} ${PCL_INCLUDE_DIRS})
target_compile_definitions(meim PUBLIC ${PCL_DEFINITIONS})
target_link_libraries(meim ARE simulatedER cmaes tbb ${PCL_LIBRARIES})

install(TARGETS meim DESTINATION lib)
install(DIRECTORY . DESTINATION include/meim FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

