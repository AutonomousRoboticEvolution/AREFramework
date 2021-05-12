cmake_minimum_required(VERSION 3.0)

find_package(Threads REQUIRED)

FILE(GLOB_RECURSE SOURCES
    "src/*"
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/common/shared_memory.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApi.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApiPlatform.c>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/shared_memory.c>
)

add_executable(ERClient ${SOURCES})
target_include_directories(ERClient PUBLIC
    "include"
    $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/>
    $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/>
)
target_link_libraries(ERClient
    ER_library
    ${CMAKE_DL_LIBS} #dlopen library
    ${CMAKE_THREAD_LIBS_INIT}
)
target_compile_definitions(ERClient PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
)

set_target_properties(ERClient PROPERTIES
    CXX_STANDARD 14
    # COMPILE_FLAGS "-w"
)

if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(ERClient PUBLIC "__WINDOWS__" "_USE_MATH_DEFINES" "DO_NOT_USE_SHARED_MEMORY")
elseif(APPLE)
    target_compile_definitions(ERClient PUBLIC "__APPLE__" "DO_NOT_USE_SHARED_MEMORY")
elseif(UNIX)
    target_compile_definitions(ERClient PUBLIC "__linux")
    target_link_libraries(ERClient rt)
endif()
