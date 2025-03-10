# Example CMAKE file on how to use NN2 when installed
cmake_minimum_required(VERSION 3.3)
project(nn2tests)

add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

# DEPENDENCIES
find_package(NN2 REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(test_cppn test_cppn.cpp)
target_link_libraries(test_cppn NN2::NN2 ${OpenCV_LIBRARIES})
target_include_directories(test_cppn PUBLIC ${OpenCV_INCLUDE_DIRS})

add_executable(test_cppn_serialization test_cppn_serialization.cpp)
target_link_libraries(test_cppn_serialization NN2::NN2 ${OpenCV_LIBRARIES})
target_include_directories(test_cppn_serialization PUBLIC ${OpenCV_INCLUDE_DIRS})

add_executable(test_cpg test_cpg.cpp)
target_link_libraries(test_cpg NN2::NN2 ${OpenCV_LIBRARIES})
target_include_directories(test_cpg PUBLIC ${OpenCV_INCLUDE_DIRS})

add_executable(test_cpg2 test_cpg2.cpp)
target_link_libraries(test_cpg2 NN2::NN2 ${OpenCV_LIBRARIES})
target_include_directories(test_cpg2 PUBLIC ${OpenCV_INCLUDE_DIRS})
