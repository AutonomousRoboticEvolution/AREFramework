cmake_minimum_required(VERSION 3.0)


add_library(reevaluateFixed SHARED 
    reevaluateFixed.cpp
    factories.cpp
    )
target_include_directories(reevaluateFixed PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "../common/"
    "/usr/include/eigen3"
    "../../modules/"
    )
target_link_libraries(reevaluateFixed ARE simulatedER)
install(TARGETS reevaluateFixed DESTINATION lib)
install(DIRECTORY . DESTINATION include/reevaluateFixed  FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

