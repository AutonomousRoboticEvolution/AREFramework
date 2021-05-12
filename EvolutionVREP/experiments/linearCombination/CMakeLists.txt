cmake_minimum_required(VERSION 3.0)


add_library(linearCombination SHARED
        LinearCombination.cpp
    CPPNIndividual.cpp
        ../common/testEnv.cpp
    factories.cpp
    ManLog.cpp)
target_include_directories(linearCombination PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    )
target_link_libraries(linearCombination ER_library)
install(TARGETS linearCombination DESTINATION lib)
