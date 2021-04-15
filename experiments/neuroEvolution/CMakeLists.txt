cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(INCLUDE     
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "../../modules"
    "/usr/include/eigen3/")

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


add_library(neuroEvolution SHARED
    ${VREP_SRC}
    basicEA.cpp
    factories.cpp)
target_include_directories(neuroEvolution PUBLIC ${INCLUDE})
target_link_libraries(neuroEvolution ARE simulatedER)

add_executable(neuroEvolution_test neuroEvolution_test.cpp)
target_include_directories(neuroEvolution_test PUBLIC ${INCLUDE})
target_link_libraries(neuroEvolution_test neuroEvolution ARE simulatedER)

install(TARGETS  neuroEvolution DESTINATION lib)
install(DIRECTORY . DESTINATION include/neuroEvolution FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
