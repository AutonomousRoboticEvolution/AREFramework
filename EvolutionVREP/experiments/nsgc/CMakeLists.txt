cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(SRC
    NSGC.cpp
    factories.cpp)


set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3/")

add_library(NSGC SHARED  ${SRC})

target_include_directories(NSGC PUBLIC ${INCLUDES})
target_link_libraries(NSGC ER_library tbb)

add_executable(nsgc_test nsgc_test.cpp)
target_include_directories(nsgc_test PUBLIC ${INCLUDES})
target_link_libraries(nsgc_test NSGC ER_library tbb)

install(TARGETS NSGC DESTINATION lib)
install(DIRECTORY . DESTINATION include/nsgc FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

