cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(SA_NIPES SHARED
   factories.cpp
   SA_NIPES.cpp
    NIPESLoggings.cpp
    )
target_include_directories(SA_NIPES PUBLIC ${INCLUDES})
target_link_libraries(SA_NIPES ER_library cmaes tbb)


install(TARGETS SA_NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/sa_nipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

