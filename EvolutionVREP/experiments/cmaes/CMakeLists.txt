cmake_minimum_required(VERSION 3.0)


add_library(CMAES SHARED 
    ../common/NNControl.cpp
    ../common/AREPuckMorphology.cpp
    ../common/EPuckMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/genLoggings.cpp
    CMAES.cpp
    CMAESIndividual.cpp
    factories.cpp
    CMAESLoggings.cpp
    )
target_include_directories(CMAES PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(CMAES ER_library cmaes)
install(TARGETS CMAES DESTINATION lib)
