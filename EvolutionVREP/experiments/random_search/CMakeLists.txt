cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
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
target_link_libraries(randomSearch ER_library cmaes)

#add_executable(nipes_test nipes_test.cpp)
#target_include_directories(nipes_test PUBLIC ${INCLUDES})
#target_link_libraries(nipes_test NIPES cmaes)

install(TARGETS randomSearch DESTINATION lib)
install(DIRECTORY . DESTINATION include/randomSearch FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
