cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

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


add_library(NIPES SHARED 
    ${VREP_SRC}
    factories.cpp
    NIPESLoggings.cpp
    NIPES.cpp
    obstacleAvoidance.cpp
    )
target_include_directories(NIPES PUBLIC ${INCLUDES})
target_link_libraries(NIPES ARE
    simulatedER cmaes tbb)

add_executable(nipes_test nipes_test.cpp)
target_include_directories(nipes_test PUBLIC ${INCLUDES})
target_link_libraries(nipes_test ARE NIPES cmaes)

install(TARGETS NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/nipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

