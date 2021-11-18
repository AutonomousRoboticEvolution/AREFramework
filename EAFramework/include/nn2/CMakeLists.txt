cmake_minimum_required(VERSION 3.0)
project(nn2)
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

message("Load nn2 module")

set(WITH_NN2 ON CACHE INTERNAL "WITH_NN2")

add_definitions(-DEIGEN3_ENABLED)

#add_executable(bench_nn bench_nn.cpp)
#target_include_directories(bench_nn PUBLIC "/usr/include/eigen3")

if(TEST_CPPN)
    find_package(Boost REQUIRED COMPONENTS serialization)
    find_package(OpenCV REQUIRED)

    add_executable(test_cppn test_cppn.cpp)
    target_link_libraries(test_cppn ${OpenCV_LIBRARIES})
    target_include_directories(test_cppn PUBLIC ${OpenCV_INCLUDE_DIRS} "/usr/include/eigen3")

    add_executable(test_cppn_serialization test_cppn_serialization.cpp)
    target_link_libraries(test_cppn_serialization ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})
    target_include_directories(test_cppn_serialization PUBLIC ${OpenCV_INCLUDE_DIRS} "/usr/include/eigen3")
endif()

install(DIRECTORY . DESTINATION include/nn2/ FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

