cmake_minimum_required(VERSION 3.0)

add_library(hyperneat_bayesO SHARED
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    ../common/CPPNGenome.cpp
    ../common/NNControl.cpp
    ../common/EPuckMorphology.cpp
    ../common/testEnv.cpp
    ../common/mazeEnv.cpp
    factories.cpp
    ../common/Loggings.cpp
    HNBOLoggings.cpp
    BOLearner.cpp
    )
target_include_directories(hyperneat_bayesO PUBLIC
    "${VREP_FOLDER}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    )
target_link_libraries(hyperneat_bayesO ER_library cmaes rt)
install(TARGETS hyperneat_bayesO DESTINATION lib)
