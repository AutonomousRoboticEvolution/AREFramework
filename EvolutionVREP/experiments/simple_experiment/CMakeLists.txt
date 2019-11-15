cmake_minimum_required(VERSION 3.0)


add_library(simple_experiment SHARED
    factories.cpp
    DummyEnv.cpp
    DummyControl.cpp
    EA_CGA.cpp)
target_include_directories(simple_experiment PUBLIC
    "include"
    "${VREP_FOLDER}/programming/include"
    )
target_link_libraries(simple_experiment ER_library)
install(TARGETS simple_experiment DESTINATION lib)
