cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

add_library(CMABO SHARED 
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/genLoggings.cpp
    ../common/Novelty.cpp
    ../common/NIPES.cpp
    ../common/BOLearner.cpp
    ../common/NN2Individual.cpp
    CMABO.cpp
    factories.cpp
    CMABOLoggings.cpp
    )
target_include_directories(CMABO PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "${LIMBO_FOLDER}/src/"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    )
target_link_libraries(CMABO ER_library cmaes)
install(TARGETS CMABO DESTINATION lib)
install(DIRECTORY . DESTINATION include/cmabo FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
