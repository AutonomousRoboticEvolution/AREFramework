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

add_library(me2im SHARED
    ${VREP_SRC}
    factories.cpp
    me2im.cpp
    me2im_logging.cpp
    ../common/fixed_controller.cpp
    ../common/obstacleAvoidance.cpp
    ../common/homeokinesis_controller.cpp
)
target_include_directories(me2im PUBLIC ${INCLUDES} ${PCL_INCLUDE_DIRS})
target_compile_definitions(me2im PUBLIC ${PCL_DEFINITIONS})
target_link_libraries(me2im ARE simulatedER cmaes tbb ${PCL_LIBRARIES})

add_executable(generate_grid_archive generate_grid_archive.cpp)
target_include_directories(generate_grid_archive PUBLIC ${INCLUDES})


install(TARGETS me2im DESTINATION lib/ARE/experiments)
install(DIRECTORY . DESTINATION include/ARE/experiements/me2im FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

