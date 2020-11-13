cmake_minimum_required(VERSION 3.0)

set(INCLUDE   $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    "../../modules/")

add_library(MFBO SHARED
    ../common/Loggings.cpp
    ../common/NNParamGenome.cpp
    ../common/NNGenome.cpp
    ../common/NN2Individual.cpp
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/testEnv.cpp
    ../common/BOLearner.cpp
    factories.cpp
    MFBO.cpp
    BOIndividual.cpp
    BOLoggings.cpp
)
target_include_directories(MFBO PUBLIC ${INCLUDE})
target_link_libraries(MFBO ER_library cmaes rt)

add_executable(mfbo_test mfbo_test.cpp)
target_link_libraries(mfbo_test MFBO ER_library cmaes rt  tbb)
target_include_directories(mfbo_test PUBLIC ${INCLUDE})

install(TARGETS MFBO DESTINATION lib)
install(DIRECTORY . DESTINATION include/mfbo FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
