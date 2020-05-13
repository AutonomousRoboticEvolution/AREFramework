cmake_minimum_required(VERSION 3.0)

add_library(MFBO SHARED
    noEA.cpp
    BOIndividual.cpp
    ../common/NNControl.cpp
    ../common/EPuckMorphology.cpp
    ../common/AREPuckMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/testEnv.cpp
    factories.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    BOLoggings.cpp
    ../common/BOLearner.cpp
    )
target_include_directories(MFBO PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    )
target_link_libraries(MFBO ER_library cmaes rt)
install(TARGETS MFBO DESTINATION lib)
install(DIRECTORY . DESTINATION include/mfbo FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
