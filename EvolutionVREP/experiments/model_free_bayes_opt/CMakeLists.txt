cmake_minimum_required(VERSION 3.0)

add_library(model_free_bayes_opt SHARED
    noEA.cpp
    BOIndividual.cpp
    ../common/NNControl.cpp
    ../common/EPuckMorphology.cpp
    ../common/mazeEnv.cpp
    factories.cpp
    Loggings.cpp
    BOLearner.cpp
    NEATGenome.cpp
    )
target_include_directories(model_free_bayes_opt PUBLIC
    "${VREP_FOLDER}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    )
target_link_libraries(model_free_bayes_opt ER_library cmaes rt)
install(TARGETS model_free_bayes_opt DESTINATION lib)
