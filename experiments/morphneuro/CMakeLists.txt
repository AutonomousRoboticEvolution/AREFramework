cmake_minimum_required(VERSION 3.0)
set(VREP_FOLDER "/home/miller/Desktop/V-REP_PRO_EDU_V3_6_2_Ubuntu18_04")

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
