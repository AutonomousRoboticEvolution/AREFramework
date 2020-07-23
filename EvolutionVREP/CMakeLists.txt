cmake_minimum_required(VERSION 3.0)
project(ER)

add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (NOT VREP_FOLDER AND NOT COPPELIASIM_FOLDER)
    message(FATAL_ERROR "VREP_FOLDER or COPPELIASIM_FOLDER variables not set, please set the variable to the vrep/coppeliasim location. Example: -DVREP_FOLDER=/usr/share/vrep/ or -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
elseif(COPPELIASIM_FOLDER)
    message("COPPELIA FOUND!")
    SET(COPPELIASIM_FOUND ON)
    SET(VREP_FOUND OFF)
    add_definitions(-DCOPPELIASIM)
    if (WIN32)
        set(VREP_LIB_FOLDER "${COPPELIASIM_FOLDER}")
        set(VREP_EXE        "${COPPELIASIM_FOLDER}/coppeliaSim.exe")
    elseif(APPLE)
        set(VREP_LIB_FOLDER "${COPPELIASIM_FOLDER}/coppeliaSim.app/Contents/MacOS/")
        set(VREP_EXE        "${COPPELIASIM_FOLDER}/coppeliaSim.app/Contents/MacOS/coppeliaSim")
    else()
        set(VREP_LIB_FOLDER "${COPPELIASIM_FOLDER}")
        set(VREP_EXE        "${COPPELIASIM_FOLDER}/coppeliaSim.sh")
    endif()

    if(NOT EXISTS ${VREP_EXE})
        message(FATAL_ERROR "COPPELIASIM_FOLDER not configure properly: \"${VREP_EXE}\"not found")
    endif()
elseif(VREP_FOLDER)
    message("VREP FOUND!")
    SET(VREP_FOUND ON)
    SET(COPPELIASIM_FOUND OFF)
    add_definitions(-DVREP)

    if (WIN32)
        set(VREP_LIB_FOLDER "${VREP_FOLDER}")
        set(VREP_EXE "${VREP_FOLDER}/vrep.exe")
    elseif(APPLE)
        set(VREP_LIB_FOLDER "${VREP_FOLDER}/vrep.app/Contents/MacOS/")
        set(VREP_EXE "${VREP_FOLDER}/vrep.app/Contents/MacOS/vrep")
    else()
        set(VREP_LIB_FOLDER "${VREP_FOLDER}")
        set(VREP_EXE "${VREP_FOLDER}/vrep.sh")
    endif()

    if(NOT EXISTS ${VREP_EXE})
        message(FATAL_ERROR "VREP_FOLDER not configure properly: \"${VREP_EXE}\"not found")
    endif()
else()
    message(FATAL_ERROR "VREP_FOLDER variable not set, please set the variable to the vrep location. Example: -DVREP_FOLDER=/usr/share/vrep/")
endif()

add_subdirectory(modules)

find_package(Boost REQUIRED random)
find_package(MultiNEAT REQUIRED)
message("MultiNEAT Folder : ${MultiNEAT_DIR}")
find_package(PolyVox REQUIRED)

set(CORE_SRC
    "src/ARE/*"
    "src/misc/*"
)

SET(VREP_SRC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/v_repLib.cpp>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/shared_memory.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/simLib.cpp>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/shared_memory.c>
    )

if(WITH_NN2 AND LIMBO_FOLDER)
    FILE(GLOB SOURCES ${CORE_SRC} "src/ARE/nn2/*" "src/ARE/learning/*")
elseif(WITH_NN2)
    FILE(GLOB SOURCES ${CORE_SRC} "src/ARE/nn2/*" "src/ARE/learning/NIPES.cpp")
else()
    FILE(GLOB SOURCES ${CORE_SRC})
endif()

message("SOURCES : ${SOURCES}")

add_library(ER_library SHARED ${VREP_SRC} ${SOURCES})

target_include_directories(ER_library PUBLIC
    "include"
    "ERClient/include"
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "/usr/include/eigen3"
    "modules"
    "${LIMBO_FOLDER}/src/"
)


target_compile_definitions(ER_library PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)


# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set_target_properties(ER_library PROPERTIES 
    CXX_STANDARD 14
#    COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

if(VREP_FOUND)
    SET(LIB_EXT v_repExtER)
elseif(COPPELIASIM_FOUND)
    SET(LIB_EXT simExtER)
endif()

add_library(${LIB_EXT} SHARED
    src/plugin/v_repExtER.cpp
)


target_link_libraries(ER_library
    MultiNEAT::MultiNEAT
    dlibxx dl rt tbb cmaes
    ${Boost_LIBRARIES}
)

target_include_directories(ER_library
    PRIVATE
    ${PolyVox_INCLUDE_DIRS}
    "modules/"
)

install(TARGETS ER_library DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

target_include_directories(${LIB_EXT} PUBLIC
    "include"
    "ERClient/include"
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
  )


set_target_properties(${LIB_EXT} PROPERTIES
    CXX_STANDARD 14
    # COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

if (COPPELIASIM_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${COPPELIASIM_FOLDER})
elseif (VREP_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${VREP_FOLDER})
endif()

if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(ER_library PUBLIC "__WINDOWS__")
elseif(APPLE)
    target_compile_definitions(ER_library PUBLIC "__APPLE__")
elseif(UNIX)
    target_compile_definitions(ER_library PUBLIC "__linux")
endif()

add_subdirectory(ERClient)
add_subdirectory(experiments)
