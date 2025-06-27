cmake_minimum_required(VERSION 3.0)

SET(VREP_SRC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/v_repLib.cpp>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/shared_memory.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include/simLib/simLib.cpp>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApi.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApiPlatform.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c>
    )

add_library(reevaluateFixed SHARED 
    ${VREP_SRC}
    reevaluateFixed.cpp
    factories.cpp
    )
target_include_directories(reevaluateFixed PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    )
target_link_libraries(reevaluateFixed ARE simulatedER)
install(TARGETS reevaluateFixed DESTINATION lib)
install(DIRECTORY . DESTINATION include/reevaluateFixed  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

