cmake_minimum_required(VERSION 3.0)

add_library(blackdrops SHARED
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
    BlackDROPS.cpp
    NEATGenome.cpp
    )
target_include_directories(blackdrops PUBLIC
    "${VREP_FOLDER}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "${LIMBO_FOLDER}/exp/blackdrops/include"
    "/usr/include/eigen3"
    "../common/"
    )
target_link_libraries(blackdrops ER_library cmaes rt)
install(TARGETS blackdrops DESTINATION lib)
