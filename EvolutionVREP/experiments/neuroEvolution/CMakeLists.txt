cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(INCLUDE     
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "../../modules"
    "/usr/include/eigen3/")

add_library(neuroEvolution SHARED
    ../common/Loggings.cpp
    basicEA.cpp
    NELoggings.cpp
    factories.cpp)
target_include_directories(neuroEvolution PUBLIC ${INCLUDE})
target_link_libraries(neuroEvolution ER_library)

add_executable(neuroEvolution_test neuroEvolution_test.cpp)
target_include_directories(neuroEvolution_test PUBLIC ${INCLUDE})
target_link_libraries(neuroEvolution_test neuroEvolution ER_library)

install(TARGETS  neuroEvolution DESTINATION lib)
install(DIRECTORY . DESTINATION include/neuroEvolution FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
