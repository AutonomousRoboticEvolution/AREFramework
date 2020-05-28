cmake_minimum_required(VERSION 3.0)


add_library(hyperneat SHARED
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    ../common/CPPNGenome.cpp
    ../common/NNControl.cpp
    ../common/testEnv.cpp
    ../common/mazeEnv.cpp
    factories.cpp
    ../common/Loggings.cpp
    HNLoggings.cpp)
target_include_directories(hyperneat PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    )
target_link_libraries(hyperneat ER_library)
install(DIRECTORY . DESTINATION include/hyperneat FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )