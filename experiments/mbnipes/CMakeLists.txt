cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    "${LIMBO_FOLDER}/src/")


add_library(MB_NIPES SHARED 
    MB_NIPES.cpp
    factories.cpp
    MB_NIPESLoggings.cpp
)


target_include_directories(MB_NIPES PUBLIC ${INCLUDES})
target_link_libraries(MB_NIPES ER_library cmaes)


#add_executable(mbnipes_test mbnipes_test.cpp)
#target_include_directories(mbnipes_test PUBLIC ${INCLUDES})
#target_link_libraries(mbnipes_test MB_NIPES cmaes tbb)

install(TARGETS MB_NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/mbnipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
