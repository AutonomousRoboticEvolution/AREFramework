cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(PM_NIPES SHARED 
    ../common/straightLineEnv.cpp
    factories.cpp
    PMNIPES.cpp
    NIPESLoggings.cpp
    )
target_include_directories(PM_NIPES PUBLIC ${INCLUDES})
target_link_libraries(PM_NIPES ER_library cmaes tbb)

install(TARGETS PM_NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/pmnipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

