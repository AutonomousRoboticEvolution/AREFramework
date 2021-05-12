cmake_minimum_required(VERSION 3.0)

set(INCLUDE   "${VREP_FOUND}/programming/include"
    "${LIMBO_FOLDER}/src/"
    "/usr/include/eigen3"
    "../common/"
    "../../modules/")

add_library(BayesOpt SHARED
    factories.cpp
    BO.cpp
    BOIndividual.cpp
    BOLoggings.cpp
)
target_include_directories(BayesOpt PUBLIC ${INCLUDE})
target_link_libraries(BayesOpt ER_library cmaes rt)

add_executable(bo_test bo_test.cpp)
target_link_libraries(bo_test BayesOpt ER_library cmaes rt  tbb)
target_include_directories(bo_test PUBLIC ${INCLUDE})

install(TARGETS BayesOpt DESTINATION lib)
install(DIRECTORY . DESTINATION include/bayesopt FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

