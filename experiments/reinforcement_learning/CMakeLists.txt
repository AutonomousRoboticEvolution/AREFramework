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


add_library(reinforcement_learning SHARED 
	${VREP_SRC}
    smarties.cpp
    factories.cpp
    rl_learner.cpp
    ../common/obstacleAvoidance.cpp
    #    ../common/homeokinesis_controller.cpp
    #../common/rl_controller.cpp
    )
target_include_directories(reinforcement_learning PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules"
    "../../modules/smarties/include"
    "../../modules/smarties/source"
    )
target_link_libraries(reinforcement_learning ARE simulatedER)
install(TARGETS reinforcement_learning DESTINATION lib)
install(DIRECTORY . DESTINATION include/reinforcement_learning  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
