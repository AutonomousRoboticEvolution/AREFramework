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

set(c10_LIBRARY "${LIBTORCH_FOLDER}/lib/")
set(kineto_LIBRARY "${LIBTORCH_FOLDER}/lib/")
set(Torch_DIR "${LIBTORCH_FOLDER}/share/cmake/Torch/")
set(Caffe2_DIR "${LIBTORCH_FOLDER}/share/cmake/Caffe2/")
find_package(Torch REQUIRED)

add_library(locomotion_ppo SHARED 
    ${VREP_SRC}
    locomotion_ppo.cpp
    factories.cpp
    ../common/locomotion.cpp
   # locomotion_ppo_env.cpp
    )
target_include_directories(locomotion_ppo PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/torchRL/torchrl/include/"
    "${TORCH_INCLUDES}"
    )
target_link_libraries(locomotion_ppo  ARE simulatedER ${TORCH_LIBRARIES} ${CMAKE_SOURCE_DIR}/build/modules/torchRL/torchrl/libtorchrl.a)
install(TARGETS locomotion_ppo DESTINATION lib)
install(DIRECTORY . DESTINATION include/locomotion_ppo FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
