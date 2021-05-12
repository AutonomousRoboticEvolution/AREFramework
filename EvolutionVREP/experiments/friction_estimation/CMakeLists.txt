cmake_minimum_required(VERSION 3.0)


add_library(fric_est SHARED 
    frictionSetup.cpp
    factories.cpp
    )
target_include_directories(fric_est PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules"
    )

target_link_libraries(fric_est ER_library cmaes)
install(TARGETS fric_est DESTINATION lib)
install(DIRECTORY . DESTINATION include/fric_est  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
