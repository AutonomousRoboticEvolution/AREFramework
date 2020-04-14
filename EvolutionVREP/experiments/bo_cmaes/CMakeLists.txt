cmake_minimum_required(VERSION 3.0)


add_library(BO_CMAES SHARED 
    ../common/NNControl.cpp
    ../common/AREPuckMorphology.cpp
    ../common/EPuckMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/genLoggings.cpp
    ../common/Novelty.cpp
    BOLearner.cpp
    BOCMAES.cpp
    BOCMAESIndividual.cpp
    factories.cpp
    BOCMAESLoggings.cpp
    )
target_include_directories(BO_CMAES PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "${LIMBO_FOLDER}/src/"
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(BO_CMAES ER_library cmaes)
install(TARGETS BO_CMAES DESTINATION lib)
