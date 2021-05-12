cmake_minimum_required(VERSION 3.0)


add_library(morphNeuro SHARED
        MorphNeuro.cpp
    CPPNIndividual.cpp
        ../common/testEnv.cpp
        ../common/NNControl.cpp
    factories.cpp
    ManLog.cpp)
target_include_directories(morphNeuro PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    )
target_link_libraries(morphNeuro ER_library)
install(TARGETS morphNeuro DESTINATION lib)
