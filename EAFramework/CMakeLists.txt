cmake_minimum_required(VERSION 3.0)
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

find_package(Boost REQUIRED COMPONENTS serialization filesystem)

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
find_package(PolyVox REQUIRED)

    find_package(MultiNEAT REQUIRED)
    message("MultiNEAT Folder : ${MultiNEAT_DIR}")

if(WITH_NN2 AND LIMBO_FOLDER)
    FILE(GLOB SOURCES 
            "src/*"
            "src/misc/*" 
            "src/nn2/*"
            "src/learning/*")
elseif(WITH_NN2)
    FILE(GLOB SOURCES 
            "src/*"
            "src/misc/*" 
            "src/nn2/*"
            "src/learning/ipop_cmaes.cpp")
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
    "${LIMBO_FOLDER}/src/"
)

target_link_libraries(ARE cmaes tbb ${Boost_LIBRARIES})

target_compile_definitions(ARE PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)

install(TARGETS ARE DESTINATION lib)
install(DIRECTORY . DESTINATION include/ARE FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set_target_properties(ARE PROPERTIES
    CXX_STANDARD 14
#    COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)
endif()
