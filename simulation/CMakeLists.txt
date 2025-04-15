cmake_minimum_required(VERSION 3.0)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (NOT COPPELIASIM_FOLDER)
    message(FATAL_ERROR "COPPELIASIM_FOLDER variables not set, please set the variable to the coppeliasim location. Example: -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
elseif(COPPELIASIM_FOLDER)
    message("COPPELIA FOUND!")
    SET(COPPELIASIM_FOUND ON CACHE BOOL "" FORCE) #CACHE BOOL "" FORCE option allows this variable to be accessed in other CMakeLists of the project
    add_definitions(-DCOPPELIASIM)
    list(APPEND CMAKE_MODULE_PATH ${COPPELIASIM_FOLDER}/programming/include/cmake)
    find_package(CoppeliaSim)
else()
    message(FATAL_ERROR "COPPELIASIM_FOLDER variables not set, please set the variable to the coppeliasim location. Example: -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
endif()
find_package(PolyVox REQUIRED)
find_package(PCL 1.14.1 REQUIRED)


FILE(GLOB COPPELIA_SRC
    ${COPPELIASIM_FOLDER}/programming/include/simStack/*.cpp
    ${COPPELIASIM_FOLDER}/programming/include/simLib/simLib.cpp
    ${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c
)

if(WITH_NN2)
    FILE(GLOB SRC src/* src/nn2/*)
else()
    FILE(GLOB SRC src/*)
endif()

add_library(simulatedER SHARED ${COPPELIA_SRC}
    ${SRC})

target_include_directories(simulatedER PUBLIC
    ${COPPELIASIM_INCLUDE_DIR}
    "zmqRemoteApi/clients/cpp/"
    ${COPPELIASIM_FOLDER}/programming/
    "include"
    "ERClient/include"
    "../EAFramework/include"
    ${PCL_INCLUDE_DIRS}
)

target_compile_definitions(simulatedER PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
    ${PCL_DEFINITIONS}
)
target_link_libraries(simulatedER
    dlibxx dl rt
    ${Boost_LIBRARIES} tbb cmaes zmq
    ARE ${PCL_LIBRARIES}
    )


SET(LIB_EXT simARE)
SET(PLUGIN_SRC coppelia_plugin/simARE.cpp)
coppeliasim_add_lua(coppelia_plugin/simARE.lua)
COPPELIASIM_ADD_ADDON(coppelia_plugin/ARE.lua)

add_library(${LIB_EXT} SHARED ${COPPELIA_SRC} ${PLUGIN_SRC})

target_include_directories(${LIB_EXT} PUBLIC
    "../include"
    ${COPPELIASIM_INCLUDE_DIR}
)

target_compile_definitions(${LIB_EXT} PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)

find_package(jsoncons REQUIRED)
FILE(GLOB ZMQREMOTEAPI_SRC ${COPPELIASIM_FOLDER}/programming/zmqRemoteApi/clients/cpp/*)

add_executable(are-client app/are_client.cpp app/client.cpp app/simulator.cpp ${COPPELIASIM_FOLDER}/programming/zmqRemoteApi/clients/cpp/RemoteAPIClient.cpp)
set_source_files_properties(RemoteAPIClient.h OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RemoteAPIObjects.h)
set_source_files_properties(RemoteAPIClient.cpp OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/RemoteAPIObjects.cpp)
target_include_directories(are-client PUBLIC
    "../include"
    "./"
    "${COPPELIASIM_FOLDER}/programming/"
    "${COPPELIASIM_FOLDER}/programming/zmqRemoteApi/clients/cpp/"
    "${jsoncons_INCLUDE_DIRS}"
)
target_compile_definitions(are-client PUBLIC "SIM_REMOTEAPICLIENT_OBJECTS")
target_link_libraries(are-client
    simulatedER ARE 
    ${CMAKE_DL_LIBS} #dlopen library
    ${CMAKE_THREAD_LIBS_INIT}
)
add_executable(nn2_cppn_serial_to_dot tools/nn2_cppn_serial_to_dot.cpp)
target_include_directories(nn2_cppn_serial_to_dot PUBLIC
    "include" "/usr/include/eigen3" "../modules"
    ${COPPELIASIM_INCLUDE_DIR})
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

install(TARGETS are-client DESTINATION bin)


#target_include_directories(${LIB_EXT}
#    PRIVATE
#    ${PolyVox_INCLUDE_DIRS}
#    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/>
#    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/>
#    "modules/"
#)
install(TARGETS  simulatedER DESTINATION lib)
install(DIRECTORY . DESTINATION include/simulatedER FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )
install(TARGETS ${LIB_EXT} DESTINATION ${COPPELIASIM_FOLDER})



if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(${LIB_EXT} PUBLIC "__WINDOWS__")
elseif(APPLE)
    target_compile_definitions(${LIB_EXT} PUBLIC "__APPLE__")
elseif(UNIX)
    target_compile_definitions(${LIB_EXT} PUBLIC "__linux")
endif()

# add_subdirectory(ERClient)

