cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

add_library(IPOP_CMAES SHARED 
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/genLoggings.cpp
    ../common/Novelty.cpp
    CMAES.cpp
    CMAESIndividual.cpp
    factories.cpp
    CMAESLoggings.cpp
    )
target_include_directories(IPOP_CMAES PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    )
target_link_libraries(IPOP_CMAES ER_library cmaes)
install(TARGETS IPOP_CMAES DESTINATION lib)
install(DIRECTORY . DESTINATION include/ipop_cmaes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
