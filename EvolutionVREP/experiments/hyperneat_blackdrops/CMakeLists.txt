cmake_minimum_required(VERSION 3.0)

add_library(hyperneat_backdrops SHARED
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    CPPNGenome.cpp
    NNControl.cpp
    EPuckMorphology.cpp
    testEnv.cpp
    factories.cpp
    Loggings.cpp
    BOLearner.cpp
    )
target_include_directories(hyperneat_backdrops PUBLIC
    "${VREP_FOLDER}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "${LIMBO_FOLDER}/exp/blackdrops/include"
    "/usr/include/eigen3"
    )
target_link_libraries(hyperneat_backdrops ER_library cmaes rt)
install(TARGETS hyperneat_backdrops DESTINATION lib)
