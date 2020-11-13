cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    "${LIMBO_FOLDER}/src/"
    )

add_library(randomSearch SHARED 
    factories.cpp
    random_search.cpp
    )
target_include_directories(randomSearch PUBLIC ${INCLUDES})
target_link_libraries(randomSearch ARE simulatedER)

install(TARGETS randomSearch DESTINATION lib)
install(DIRECTORY . DESTINATION include/randomSearch FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
