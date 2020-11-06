cmake_minimum_required(VERSION 3.0)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (NOT VREP_FOLDER AND NOT COPPELIASIM_FOLDER)
    message(FATAL_ERROR "VREP_FOLDER or COPPELIASIM_FOLDER variables not set, please set the variable to the vrep/coppeliasim location. Example: -DVREP_FOLDER=/usr/share/vrep/ or -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
elseif(COPPELIASIM_FOLDER)
    message("COPPELIA FOUND!")
    SET(COPPELIASIM_FOUND ON CACHE BOOL "" FORCE) #CACHE BOOL "" FORCE option allows this variable to be accessed in other CMakeLists of the project
    SET(VREP_FOUND OFF CACHE BOOL "" FORCE)
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
    SET(VREP_FOUND ON CACHE BOOL "" FORCE)
    SET(COPPELIASIM_FOUND OFF CACHE BOOL "" FORCE)
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
    message(FATAL_ERROR "(should never reach here) VREP_FOLDER or COPPELIASIM_FOLDER variables not set, please set the variable to the vrep/coppeliasim location. Example: -DVREP_FOLDER=/usr/share/vrep/ or -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
endif()

add_subdirectory(modules)
add_subdirectory(image_processing)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED random)
find_package(MultiNEAT REQUIRED)
message("MultiNEAT Folder : ${MultiNEAT_DIR}")
find_package(PolyVox REQUIRED)



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

if(WITH_NN2)
    FILE(GLOB SRC src/* src/nn2/*)
else()
    FILE(GLOB SRC src/*)
endif()

add_library(simulatedER SHARED  ${VREP_SRC} ${SRC})
target_include_directories(simulatedER PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    "include"
    "ERClient/include"
    "../EAFramework/include")
target_link_libraries(simulatedER
    MultiNEAT::MultiNEAT
    dlibxx dl rt tbb cmaes
    ${Boost_LIBRARIES}
    ARE)

if(VREP_FOUND)
    SET(LIB_EXT v_repExtER)
    SET(PLUGIN_SRC src/plugin/v_repExtER.cpp)
elseif(COPPELIASIM_FOUND)
    SET(LIB_EXT simExtER)
    SET(PLUGIN_SRC src/plugin/simExtER.cpp)
endif()



add_library(${LIB_EXT} SHARED ${VREP_SRC} ${PLUGIN_SRC})

target_include_directories(${LIB_EXT} PUBLIC
    "include"
    "ERClient/include"
    "image_processing/include"
    ${OpenCV_INCLUDE_DIRS}
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
)


target_compile_definitions(${LIB_EXT} PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)


# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set_target_properties(${LIB_EXT} PROPERTIES 
    CXX_STANDARD 14
#    COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${LIB_EXT}
    MultiNEAT::MultiNEAT
    dlibxx dl rt tbb cmaes
    ${Boost_LIBRARIES}
    ARE
    simulatedER
)

target_include_directories(${LIB_EXT}
    PRIVATE
    ${PolyVox_INCLUDE_DIRS}
    "modules/"
)
install(TARGETS  simulatedER DESTINATION lib)
install(DIRECTORY . DESTINATION include/simulatedER FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

if (COPPELIASIM_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${COPPELIASIM_FOLDER})
elseif (VREP_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${VREP_FOLDER})
endif()


if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(${LIB_EXT} PUBLIC "__WINDOWS__")
elseif(APPLE)
    target_compile_definitions(${LIB_EXT} PUBLIC "__APPLE__")
elseif(UNIX)
    target_compile_definitions(${LIB_EXT} PUBLIC "__linux")
endif()

add_subdirectory(ERClient)

