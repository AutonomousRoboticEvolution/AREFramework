cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

find_package(OpenCV REQUIRED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    ${OpenCV_INCLUDE_DIRS}
    "../../EAFramework/include"
    "../../PhysicalER/include"
    "../../simulation/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(test_are_generate SHARED
   factories.cpp
   physicalMorphoEvo.cpp
    )

target_include_directories(test_are_generate PUBLIC ${INCLUDES})
target_link_libraries(test_are_generate ${OpenCV_LIBRARIES} ARE simulatedER)

install(TARGETS test_are_generate DESTINATION lib)
install(DIRECTORY . DESTINATION include/test_are_generate FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

