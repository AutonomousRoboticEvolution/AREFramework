cmake_minimum_required(VERSION 3.0)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (NOT VREP_FOLDER AND NOT COPPELIASIM_FOLDER)
    message(FATAL_ERROR "VREP_FOLDER or COPPELIASIM_FOLDER variables not set, please set the variable to the vrep/coppeliasim location. Example: -DVREP_FOLDER=/usr/share/vrep/ or -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
elseif(COPPELIASIM_FOLDER)
    message("COPPELIA FOUND!")
    SET(COPPELIASIM_FOUND ON CACHE BOOL "" FORCE) #CACHE BOOL "" FORCE option allows this variable to be accessed in other CMakeLists of the project
    SET(VREP_FOUND OFF CACHE BOOL "" FORCE)
    add_definitions(-DCOPPELIASIM)
    if(NOT LIBPLUGIN_DIR)
         set(LIBPLUGIN_DIR ${COPPELIASIM_FOLDER}/programming/libPlugin)
    endif()
    list(APPEND CMAKE_MODULE_PATH ${LIBPLUGIN_DIR}/cmake)
    find_package(CoppeliaSim 4.3.0.0 REQUIRED)
#    if (WIN32)
#        set(VREP_LIB_FOLDER "${COPPELIASIM_FOLDER}")
#        set(VREP_EXE        "${COPPELIASIM_FOLDER}/coppeliaSim.exe")
#    elseif(APPLE)
#        set(VREP_LIB_FOLDER "${COPPELIASIM_FOLDER}/coppeliaSim.app/Contents/MacOS/")
#        set(VREP_EXE        "${COPPELIASIM_FOLDER}/coppeliaSim.app/Contents/MacOS/coppeliaSim")
#    else()
#        set(VREP_LIB_FOLDER "${COPPELIASIM_FOLDER}")
#        set(VREP_EXE        "${COPPELIASIM_FOLDER}/coppeliaSim.sh")
#    endif()

#    if(NOT EXISTS ${VREP_EXE})
#        message(FATAL_ERROR "COPPELIASIM_FOLDER not configure properly: \"${VREP_EXE}\"not found")
#    endif()
elseif(VREP_FOLDER)
    message("Using VREP is deprecated")
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
find_package(PolyVox REQUIRED)
find_package(PCL 1.14.1 REQUIRED)

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

add_library(simulatedER SHARED ${VREP_SRC} ${SRC})
target_include_directories(simulatedER PUBLIC
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
#    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_INCLUDE_DIR}>
    "include"
    "ERClient/include"
    "../EAFramework/include"
    ${PCL_INCLUDE_DIRS}
)

target_compile_definitions(simulatedER PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)
target_link_libraries(simulatedER
    dlibxx dl rt
    ${Boost_LIBRARIES} tbb cmaes zmq
    ARE ${PCL_LIBRARIES}
    )


# set_target_properties(simulatedER PROPERTIES
#     CXX_STANDARD 17
# #    COMPILE_FLAGS "-w"
#     POSITION_INDEPENDENT_CODE ON
# )

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
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_INCLUDE_DIR}>
)


target_compile_definitions(${LIB_EXT} PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)

add_executable(nn2_cppn_serial_to_dot tools/nn2_cppn_serial_to_dot.cpp)
target_include_directories(nn2_cppn_serial_to_dot PUBLIC
    "include" "/usr/include/eigen3" "../modules"
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_INCLUDE_DIR}>)
target_link_libraries(nn2_cppn_serial_to_dot ARE ${Boost_LIBRARIES} simulatedER)
install(TARGETS nn2_cppn_serial_to_dot DESTINATION bin)

# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")


#set_target_properties(${LIB_EXT} PROPERTIES
#    CXX_STANDARD 14
###    COMPILE_FLAGS "-w"
#    POSITION_INDEPENDENT_CODE ON
#)
#message("${COPPELIASIM_LIBRARIES}")

target_link_libraries(${LIB_EXT}
    dlibxx dl #rt
    ${Boost_LIBRARIES}
    ARE
    simulatedER
)



target_include_directories(${LIB_EXT}
    PRIVATE
    ${PolyVox_INCLUDE_DIRS}
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/>
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

