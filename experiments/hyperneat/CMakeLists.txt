cmake_minimum_required(VERSION 3.0)


add_library(hyperneat SHARED
    ../common/NNControl.cpp
    ../common/testEnv.cpp
    EA_HyperNEAT.cpp
    CPPNIndividual.cpp
    factories.cpp)
target_include_directories(hyperneat PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
<<<<<<< HEAD:EvolutionVREP/experiments/hyperneat/CMakeLists.txt
=======
    "../../simulation/include"
    "../../EAFramework/include"
>>>>>>> real_experiment:experiments/hyperneat/CMakeLists.txt
    "../common/"
    )
target_link_libraries(hyperneat ARE simulatedER)
install(TARGETS  hyperneat DESTINATION lib)
install(DIRECTORY . DESTINATION include/hyperneat FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
