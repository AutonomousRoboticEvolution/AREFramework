cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(INCLUDES
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "../../modules"
    "/usr/include/eigen3/")

add_library(NSLC SHARED
    NSLC.cpp
    factories.cpp)
target_include_directories(NSLC PUBLIC ${INCLUDES})
target_link_libraries(NSLC ER_library tbb)


add_executable(nslc_test nslc_test.cpp)
target_include_directories(nslc_test PUBLIC ${INCLUDES})
target_link_libraries(nslc_test NSLC ER_library)

install(TARGETS  NSLC DESTINATION lib)
install(DIRECTORY . DESTINATION include/nslc FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
