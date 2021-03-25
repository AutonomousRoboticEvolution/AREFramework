cmake_minimum_required(VERSION 3.0)


add_library(bodyplantesting SHARED
    ../common/testEnv.cpp
    BodyPlanTesting.cpp
    CPPNIndividual.cpp
    factories.cpp
    ManLog.cpp ../../include/ARE/Organ.h ../../src/ARE/Organ.cpp ../../include/ARE/GenomeDecoder.h ../../src/ARE/GenomeDecoder.cpp)
target_include_directories(bodyplantesting PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(bodyplantesting ER_library)
install(TARGETS bodyplantesting DESTINATION lib)
