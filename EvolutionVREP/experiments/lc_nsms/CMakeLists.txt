cmake_minimum_required(VERSION 3.0)


add_library(lc_nsms SHARED
    LC_NSMS.cpp
    CPPNIndividual.cpp
    ../common/CPPNGenome.cpp
    ../common/testEnv.cpp
    factories.cpp
    ../common/Novelty.cpp
    ManLog.cpp)
target_include_directories(lc_nsms PUBLIC
    "${VREP_FOLDER}/programming/include"
    "../common/"
    )
target_link_libraries(lc_nsms ER_library)
install(TARGETS lc_nsms DESTINATION lib)
