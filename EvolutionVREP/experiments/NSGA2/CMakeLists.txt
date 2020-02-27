cmake_minimum_required(VERSION 3.0)


add_library(NSGA2 SHARED 
    ../common/NNControl.cpp
    ../common/EPuckMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    NSGA2.cpp
    NSGAIndividual.cpp
    factories.cpp
    NELoggings.cpp
    )
target_include_directories(NSGA2 PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    )
target_link_libraries(NSGA2 ER_library)
install(TARGETS NSGA2 DESTINATION lib)
