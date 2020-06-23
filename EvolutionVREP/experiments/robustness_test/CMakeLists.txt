cmake_minimum_required(VERSION 3.0)


add_library(robustnessTest SHARED 
    ../common/NNControl.cpp
    ../common/FixedMorphology.cpp
    ../common/mazeEnv.cpp
    ../common/Loggings.cpp
    ../common/genLoggings.cpp
    ../common/NNGenome.cpp
    ../common/NNParamGenome.cpp
    ../common/NN2Individual.cpp
    robustness_test.cpp
    factories.cpp
    )
target_include_directories(robustnessTest PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    )
target_link_libraries(robustnessTest ER_library cmaes)
install(TARGETS robustnessTest DESTINATION lib)
install(DIRECTORY . DESTINATION include/robustnessTest  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
