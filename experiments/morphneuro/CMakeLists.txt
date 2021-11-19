cmake_minimum_required(VERSION 3.0)

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

add_library(morphNeuro SHARED
        ${VREP_SRC}
        MorphNeuro.cpp
    CPPNIndividual.cpp
        ../common/NNControl.cpp
    factories.cpp
    ManLog.cpp
    mazeEnv_multiTarget.cpp)
target_include_directories(morphNeuro PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "../../simulation/include"
    "../../EAFramework/include"
    "/usr/include/eigen3"
    )
target_link_libraries(morphNeuro ARE simulatedER)
install(TARGETS morphNeuro DESTINATION lib)
