cmake_minimum_required(VERSION 3.0)
set(VREP_FOLDER "/home/wei/Softwares/V-REP_PRO_EDU_V3_6_2_Ubuntu16_04")
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

add_library(hyperneat SHARED
    ${VREP_SRC}
    ../common/NNControl.cpp
    ../common/testEnv.cpp
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    factories.cpp)
target_include_directories(hyperneat PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    )
target_link_libraries(hyperneat ARE simulatedER)
install(TARGETS  hyperneat DESTINATION lib)
install(DIRECTORY . DESTINATION include/hyperneat FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
