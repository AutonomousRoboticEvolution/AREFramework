cmake_minimum_required(VERSION 3.0)


add_library(lc_nsms SHARED
    ../common/testEnv.cpp
    LC_NSMS.cpp
    CPPNIndividual.cpp
    factories.cpp
    ManLog.cpp)
target_include_directories(lc_nsms PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(lc_nsms ARE simulatedER)
install(TARGETS lc_nsms DESTINATION lib)
