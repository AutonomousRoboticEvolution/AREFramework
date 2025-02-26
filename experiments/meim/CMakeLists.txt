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


add_library(meim SHARED
    ${COPPELIA_SRC}
    factories.cpp
    meim.cpp
    meim_logging.cpp
    ../common/fixed_controller.cpp
    ../common/obstacleAvoidance.cpp
    ../common/homeokinesis_controller.cpp
  )
target_include_directories(meim PUBLIC ${INCLUDES} ${PCL_INCLUDE_DIRS})
target_compile_definitions(meim PUBLIC ${PCL_DEFINITIONS})
target_link_libraries(meim ARE simulatedER cmaes tbb ${PCL_LIBRARIES})

install(TARGETS meim DESTINATION lib/ARE/experiments)
install(DIRECTORY . DESTINATION include/ARE/experiements/meim FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

