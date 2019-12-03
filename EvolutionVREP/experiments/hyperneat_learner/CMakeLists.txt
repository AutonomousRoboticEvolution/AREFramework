cmake_minimum_required(VERSION 3.0)


add_library(hyperneat_learner SHARED
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    CPPNGenome.cpp
    NNControl.cpp
    EPuckMorphology.cpp
    testEnv.cpp
    factories.cpp
    Loggings.cpp
    BOLearner.cpp)
target_include_directories(hyperneat_learner PUBLIC
    "${VREP_FOLDER}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "${LIMBO_FOLDER}/exp/blackdrops/include"
    "/usr/include/eigen3"
    )
target_link_libraries(hyperneat_learner ER_library)
install(TARGETS hyperneat_learner DESTINATION lib)
