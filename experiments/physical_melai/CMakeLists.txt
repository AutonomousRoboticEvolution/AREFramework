cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

find_package(OpenCV REQUIRED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    ${OpenCV_INCLUDE_DIRS}
    "../../EAFramework/include"
    "../../PhysicalER/include"
    "../../simulation/include"
    "../../image_processing/include"
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

add_library(physical_melai SHARED
    ${VREP_SRC}
   factories.cpp
   mnipes.cpp
   cmaes_learner.cpp
   mnipes_loggings.cpp
   ../common/exploration.cpp
   )


target_include_directories(physical_melai PUBLIC ${INCLUDES})
target_link_libraries(physical_melai ARE  ${OpenCV_LIBRARIES}  AREImageProc simulatedER zmq)

#add_library(physical_melai_update SHARED
#   factories.cpp
#   mnipes.cpp
#   cmaes_learner.cpp
#   ../../PhysicalER/src/io_helpers.cpp
#   )
#target_compile_definitions(physical_melai_update PUBLIC UPDATE)
#target_include_directories(physical_melai_update PUBLIC ${INCLUDES})
#target_link_libraries(physical_melai_update ARE  ${OpenCV_LIBRARIES} AREImageProc simulatedER)

install(TARGETS physical_melai DESTINATION lib)
install(DIRECTORY . DESTINATION include/physical_melai FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

