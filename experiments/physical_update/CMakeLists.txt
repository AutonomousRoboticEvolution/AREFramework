cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

find_package(OpenCV REQUIRED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    ${OpenCV_INCLUDE_DIRS}
    "../../EAFramework/include"
    "../../PhysicalER/include"
    "../../PhysicalER/image_processing/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(physical_update SHARED 
   factories.cpp
   realMaze.cpp
   random_controller.cpp
    )

target_include_directories(physical_update PUBLIC ${INCLUDES})
target_link_libraries(physical_update ${OpenCV_LIBRARIES} ARE AREImageProc physicalER)

install(TARGETS physical_update DESTINATION lib)
install(DIRECTORY . DESTINATION include/physical_update FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
