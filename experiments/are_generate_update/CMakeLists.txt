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
    "../../PhysicalER/image_processing/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(are_generate_update SHARED
   factories.cpp
   realMaze.cpp
   generateUpdate.cpp
    )

target_include_directories(are_generate_update PUBLIC ${INCLUDES})
target_link_libraries(are_generate_update ${OpenCV_LIBRARIES} ARE AREImageProc simulatedER)

install(TARGETS are_generate_update DESTINATION lib)
install(DIRECTORY . DESTINATION include/are_generate_update FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

