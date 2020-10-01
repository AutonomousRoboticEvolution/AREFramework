cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(M_NIPES SHARED 
   factories.cpp
   M_NIPES.cpp 
   cmaes_learner.cpp
   MNIPESLoggings.cpp
    )
target_include_directories(M_NIPES PUBLIC ${INCLUDES})
target_link_libraries(M_NIPES ER_library cmaes tbb)

install(TARGETS M_NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/mnipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

