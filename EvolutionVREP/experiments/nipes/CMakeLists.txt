cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/")

add_library(NIPES SHARED 
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNParamGenome.cpp
    ../common/NN2Individual.cpp
    ../common/NNGenome.cpp
    ../common/genLoggings.cpp
    ../common/Novelty.cpp
    ../common/NIPES.cpp
    factories.cpp
    NIPESLoggings.cpp
    )
target_include_directories(NIPES PUBLIC ${INCLUDES})
target_link_libraries(NIPES ER_library cmaes tbb)

add_executable(nipes_test nipes_test.cpp)
target_include_directories(nipes_test PUBLIC ${INCLUDES})
target_link_libraries(nipes_test NIPES cmaes)

install(TARGETS NIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/nipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

