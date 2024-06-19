cmake_minimum_required(VERSION 3.0)

find_package(limbo)

set(INCLUDE
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "../../simulation/include"
    "../../EAFramework/include"
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    "../../modules/")

add_library(BayesOpt SHARED
    factories.cpp
    BO.cpp
    BOIndividual.cpp
    BOLoggings.cpp
    BayesianOpt.cpp
)
target_include_directories(BayesOpt PUBLIC ${INCLUDE})
target_link_libraries(BayesOpt ARE simulatedER cmaes rt ${LIMBO_LIBRAIRIES})

add_executable(bo_test bo_test.cpp)
target_link_libraries(bo_test BayesOpt ARE simulatedER cmaes rt  tbb  ${LIMBO_LIBRAIRIES})
target_include_directories(bo_test PUBLIC ${INCLUDE})

install(TARGETS BayesOpt DESTINATION lib)
install(DIRECTORY . DESTINATION include/bayesopt FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

