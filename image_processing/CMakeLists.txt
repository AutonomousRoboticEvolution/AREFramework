cmake_minimum_required(VERSION 3.0)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(OpenCV REQUIRED)

add_library(AREImageProc SHARED 
         src/blob_tracking.cpp
         src/realMaze.cpp)

target_include_directories(AREImageProc PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "include"
    "../EAFramework/include")
target_link_libraries(AREImageProc ARE ${OpenCV_LIBRARIES})

add_executable(test_blob_tracking src/test_blob_tracking.cpp src/blob_tracking.cpp)
target_include_directories(test_blob_tracking PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "include"
    "../EAFramework/include/"
    )
target_link_libraries(test_blob_tracking ARE ${OpenCV_LIBRARIES})


install(TARGETS AREImageProc DESTINATION lib)
install(DIRECTORY . DESTINATION include/image_processing FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
