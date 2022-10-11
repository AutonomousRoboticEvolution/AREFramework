cmake_minimum_required(VERSION 3.0)

find_package(Boost 1.77 REQUIRED COMPONENTS serialization filesystem)

if (RASPBERRY)
    if(WITH_NN2)
        FILE(GLOB SOURCES
            "src/Settings.cpp"
            "src/Individual.cpp"
            "src/Genome.cpp"
            "src/NNParamGenome.cpp"
            "src/Logging.cpp"
            "src/misc/*"
            "src/nn2/*")
    else()
        FILE(GLOB SOURCES
            "src/Settings.cpp"
            "src/Individual.cpp"
            "src/Genome.cpp"
            "src/Logging.cpp"
            "src/NNParamGenome.cpp"
            "src/misc/*")
    endif()

    add_library(piARE SHARED ${SOURCES})

    target_include_directories(piARE PUBLIC
        "include"
        "/usr/include/eigen3"
        "../modules"
    )

    target_link_libraries(piARE ${Boost_LIBRARIES})
else()

    find_package(MultiNEAT REQUIRED)
    message("MultiNEAT Folder : ${MultiNEAT_DIR}")


if(WITH_NN2)
    FILE(GLOB SOURCES 
            "src/*"
            "src/misc/*" 
            "src/nn2/*"
            "src/learning/ipop_cmaes.cpp"
            "src/learning/cmaes_learner.cpp"
            "src/learning/controller_archive.cpp"
            )
else()
    FILE(GLOB SOURCES    
            "src/*"
            "src/misc/*" )
endif()

add_library(ARE SHARED ${SOURCES})
target_include_directories(ARE PUBLIC
    "include"
    "/usr/include/eigen3"
    "../modules"
)
target_link_libraries(ARE MultiNEAT::MultiNEAT cmaes tbb ${Boost_LIBRARIES})

add_executable(nn2_cppn_serial_to_dot tools/nn2_cppn_serial_to_dot.cpp)
target_include_directories(nn2_cppn_serial_to_dot PUBLIC
    "include" "/usr/include/eigen3" "../modules")
target_link_libraries(nn2_cppn_serial_to_dot ARE ${Boost_LIBRARIES})

install(TARGETS nn2_cppn_serial_to_dot DESTINATION bin)
install(TARGETS ARE DESTINATION lib)
install(DIRECTORY . DESTINATION include/ARE FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )



# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")


endif()
