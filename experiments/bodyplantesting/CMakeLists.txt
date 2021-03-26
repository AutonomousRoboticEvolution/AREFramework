cmake_minimum_required(VERSION 3.0)


add_library(bodyplantesting SHARED
    BodyPlanTesting.cpp
    CPPNIndividual.cpp
    factories.cpp
    ManLog.cpp)
target_include_directories(bodyplantesting PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "${LIMBO_FOLDER}/src/"
    "../../EAFramework/include/"
    "../../simulation/include/"
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(bodyplantesting ARE)
install(TARGETS bodyplantesting DESTINATION lib)
