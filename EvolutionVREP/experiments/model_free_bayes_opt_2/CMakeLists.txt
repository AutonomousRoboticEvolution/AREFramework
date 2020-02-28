cmake_minimum_required(VERSION 3.0)

add_library(model_free_bayes_opt_2 SHARED
    noEA.cpp
    BOIndividual.cpp
    ../common/NNControl.cpp
    ../common/EPuckMorphology.cpp
    ../common/AREPuckMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/testEnv.cpp
    factories.cpp
    ../common/Loggings.cpp
    BOLoggings.cpp
    BOLearner.cpp
    NEATGenome.cpp
    )
target_include_directories(model_free_bayes_opt_2 PUBLIC
    "${VREP_FOLDER}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    )
target_link_libraries(model_free_bayes_opt_2 ER_library cmaes rt)
install(TARGETS model_free_bayes_opt_2 DESTINATION lib)
