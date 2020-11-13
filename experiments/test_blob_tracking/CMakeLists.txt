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

add_library(test_blob_tracking SHARED 
   factories.cpp
   realMaze.cpp
   blob_tracker.cpp
   #   TrackingLoggings.cpp
    )

target_include_directories(test_blob_tracking PUBLIC ${INCLUDES})
target_link_libraries(test_blob_tracking ${OpenCV_LIBRARIES} ARE AREImageProc)

install(TARGETS test_blob_tracking DESTINATION lib)
install(DIRECTORY . DESTINATION include/test_blob_tracking FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
