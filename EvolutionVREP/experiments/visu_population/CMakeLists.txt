cmake_minimum_required(VERSION 3.0)


add_library(visu_pop SHARED 
    ../common/NNControl.cpp
    ../common/AREPuckMorphology.cpp
    ../common/EPuckMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/genLoggings.cpp
    ../common/NNGenome.cpp
    visuPop.cpp
    factories.cpp
    )
target_include_directories(visu_pop PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(visu_pop ER_library cmaes)
install(TARGETS visu_pop DESTINATION lib)
