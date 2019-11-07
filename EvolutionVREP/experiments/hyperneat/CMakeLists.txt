cmake_minimum_required(VERSION 3.0)


add_library(hyperneat SHARED
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    EPuckMorphology.cpp
    testEnv.cpp)
target_include_directories(hyperneat PUBLIC
    "${VREP_FOLDER}/programming/include"
    )
target_link_libraries(hyperneat ER_library)
install(TARGETS hyperneat DESTINATION lib)
