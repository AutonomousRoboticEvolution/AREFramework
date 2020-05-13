cmake_minimum_required(VERSION 3.0)

add_library(MFBO SHARED
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/NNControl.cpp
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/testEnv.cpp
    ../common/BOLearner.cpp
    factories.cpp
    MFBO.cpp
    BOIndividual.cpp
    BOLoggings.cpp
)
target_include_directories(MFBO PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    "../../modules/"
    )
target_link_libraries(MFBO ER_library cmaes rt)
install(TARGETS MFBO DESTINATION lib)
install(DIRECTORY . DESTINATION include/mfbo FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
