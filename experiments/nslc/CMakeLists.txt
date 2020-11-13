cmake_minimum_required(VERSION 3.0)

add_definitions(-DEIGEN3_ENABLED)

set(INCLUDES
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
<<<<<<< HEAD:EvolutionVREP/experiments/nslc/CMakeLists.txt
=======
    "../../simulation/include"
    "../../EAFramework/include"
>>>>>>> real_experiment:experiments/nslc/CMakeLists.txt
    "../common/"
    "../../modules"
    "/usr/include/eigen3/")

add_library(NSLC SHARED
    NSLC.cpp
    factories.cpp)
target_include_directories(NSLC PUBLIC ${INCLUDES})
target_link_libraries(NSLC ARE simulatedER tbb)


add_executable(nslc_test nslc_test.cpp)
target_include_directories(nslc_test PUBLIC ${INCLUDES})
target_link_libraries(nslc_test NSLC ARE simulatedER)

install(TARGETS  NSLC DESTINATION lib)
install(DIRECTORY . DESTINATION include/nslc FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

