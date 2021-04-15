cmake_minimum_required(VERSION 3.0)

add_definitions(-DNDEBUG -DEIGEN3_ENABLED)

add_library(MONIPES SHARED 
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/NNGenome.cpp
    ../common/genLoggings.cpp
    ../common/Novelty.cpp
    ../common/NIPES.cpp
    factories.cpp
    MONIPESLoggings.cpp
    MONIPES.cpp
    )
target_include_directories(MONIPES PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    )
target_link_libraries(MONIPES ER_library cmaes)
install(TARGETS MONIPES DESTINATION lib)
install(DIRECTORY . DESTINATION include/monipes FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
