cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(INCLUDES
    ${COPPELIASIM_INCLUDE_DIR}
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")



SET(COPPELIA_SRC
    ${COPPELIASIM_FOLDER}/programming/include/simLib/simLib.cpp
    ${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApi.c
    ${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApiPlatform.c
    ${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c
)

add_library(me2im SHARED
    ${COPPELIA_SRC}
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

