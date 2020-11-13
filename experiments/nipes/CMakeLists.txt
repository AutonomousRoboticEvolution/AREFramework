cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(NIPES SHARED 
   factories.cpp
    NIPESLoggings.cpp
    NIPES.cpp
    )
target_include_directories(NIPES PUBLIC ${INCLUDES})
target_link_libraries(NIPES ARE simulatedER cmaes tbb)

add_executable(nipes_test nipes_test.cpp)
target_include_directories(nipes_test PUBLIC ${INCLUDES})
target_link_libraries(nipes_test NIPES cmaes)

install(TARGETS NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/nipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

