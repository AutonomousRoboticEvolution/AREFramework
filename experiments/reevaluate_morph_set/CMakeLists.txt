cmake_minimum_required(VERSION 3.0)


add_library(reevaluate_morph_set SHARED 
    reevaluate_morph_set.cpp
    factories.cpp
    )
target_include_directories(reevaluateFixed PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    )
target_link_libraries(reevaluate_morph_set ER_library)
install(TARGETS reevaluate_morph_set DESTINATION lib)
install(DIRECTORY . DESTINATION include/reevaluate_morph_set FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

