cmake_minimum_required(VERSION 3.0)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(OpenCV REQUIRED)

add_library(AREImageProc SHARED
         src/real_environment.cpp
         ../PhysicalER/src/pi_communication.cpp)

target_include_directories(AREImageProc PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "include"
    "../EAFramework/include"
    "../PhysicalER/include")
target_link_libraries(AREImageProc zmq ARE ${OpenCV_LIBRARIES})

#add_executable(test_blob_tracking src/test_blob_tracking.cpp src/blob_tracking.cpp)
#target_include_directories(test_blob_tracking PUBLIC
#    ${OpenCV_INCLUDE_DIRS}
#    "include"
#    "../EAFramework/include/"
#    )
#target_link_libraries(test_blob_tracking ARE ${OpenCV_LIBRARIES})



add_executable(test_camera_subscriber src/test_camera_subscriber.cpp ../PhysicalER/src/pi_communication.cpp)
target_include_directories(test_camera_subscriber PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "include"
    "../PhysicalER/include/"
    )
target_link_libraries(test_camera_subscriber zmq)


install(TARGETS AREImageProc DESTINATION lib)
install(DIRECTORY . DESTINATION include/image_processing FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
