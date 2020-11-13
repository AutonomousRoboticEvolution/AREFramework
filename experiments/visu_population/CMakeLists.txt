cmake_minimum_required(VERSION 3.0)


add_library(visu_pop SHARED 
    visuPop.cpp
    factories.cpp
    )
target_include_directories(visu_pop PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules"
    )
target_link_libraries(visu_pop ER_library cmaes)
install(TARGETS visu_pop DESTINATION lib)
install(DIRECTORY . DESTINATION include/visu_population  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
