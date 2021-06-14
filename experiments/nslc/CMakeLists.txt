cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

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


set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "../../modules"
    "/usr/include/eigen3/")

add_library(NSLC SHARED
    ${VREP_SRC}
    NSLC.cpp
    factories.cpp)
target_include_directories(NSLC PUBLIC ${INCLUDES})
target_link_libraries(NSLC ARE simulatedER)


add_executable(nslc_test nslc_test.cpp)
target_include_directories(nslc_test PUBLIC ${INCLUDES})
target_link_libraries(nslc_test NSLC ARE simulatedER)

install(TARGETS  NSLC DESTINATION lib)
install(DIRECTORY . DESTINATION include/nslc FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

