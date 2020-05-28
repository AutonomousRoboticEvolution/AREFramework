cmake_minimum_required(VERSION 3.0)


add_library(neuroEvolution SHARED
    ../common/NNControl.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/NNIndividual.cpp
    basicEA.cpp
    NELoggings.cpp
    factories.cpp)
target_include_directories(neuroEvolution PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    )
target_link_libraries(neuroEvolution ER_library)
install(DIRECTORY . DESTINATION include/neuroEvolution FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )