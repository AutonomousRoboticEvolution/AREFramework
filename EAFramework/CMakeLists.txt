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
            "src/genetic_operators.cpp"
            "src/sq_cppn_genome.cpp"
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



if(WITH_NN2)
    FILE(GLOB SOURCES 
            "src/*"
            "src/misc/*" 
            "src/nn2/*"
            "src/learning/ipop_cmaes.cpp"
            "src/learning/cmaes_learner.cpp"
            "src/learning/controller_archive.cpp"
            "src/learning/Novelty.cpp"

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
    "/usr/local/include/"
    "../modules"
)


target_link_libraries(ARE cmaes tbb ${Boost_LIBRARIES})



install(TARGETS ARE DESTINATION lib)
install(DIRECTORY . DESTINATION include/ARE FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )



# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")


endif()
