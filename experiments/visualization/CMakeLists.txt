cmake_minimum_required(VERSION 3.0)


add_library(visualization SHARED 
    visualization.cpp
    factories.cpp
    )
target_include_directories(visualization PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules"
    )
target_link_libraries(visualization ARE simulatedER)
install(TARGETS visualization DESTINATION lib)
install(DIRECTORY . DESTINATION include/visu_population  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
