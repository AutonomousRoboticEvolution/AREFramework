cmake_minimum_required(VERSION 3.0)


add_library(NSLC SHARED
    ../common/NNControl.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/Novelty.cpp
    ../common/NNIndividual.cpp
    NSLC.cpp
    factories.cpp
    NSLCLoggings.cpp
    )
target_include_directories(NSLC PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    "/usr/include/eigen3/"
    )
target_link_libraries(NSLC ER_library)
install(DIRECTORY . DESTINATION include/nslc FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )