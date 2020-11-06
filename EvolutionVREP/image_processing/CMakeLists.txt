cmake_minimum_required(VERSION 3.0)

find_package(OpenCV REQUIRED)

add_library(AREImageProc SHARED 
	 src/blob_tracking.cpp)

target_include_directories(AREImageProc PUBLIC ${OpenCV_INCLUDE_DIRS} include)
target_link_libraries(AREImageProc ${OpenCV_LIBRARIES})
