cmake_minimum_required(VERSION 3.0)

find_package(Threads REQUIRED)

add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")

FILE(GLOB_RECURSE SOURCES
    "src/*"
    ${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApi.c
    ${COPPELIASIM_FOLDER}/programming/legacyRemoteApi/remoteApi/extApiPlatform.c
    ${COPPELIASIM_FOLDER}/programming/include/simLib/shared_memory.c
)

add_executable(are-client ${SOURCES})
target_include_directories(are-client PUBLIC
    "include"
    "../include"
    ${COPPELIASIM_FOLDER}/programming/
)

target_link_libraries(are-client
    simulatedER ARE
    ${CMAKE_DL_LIBS} #dlopen library
    ${CMAKE_THREAD_LIBS_INIT}
)

target_compile_definitions(are-client PUBLIC
    "NON_MATLAB_PARSING"
    "MAX_EXT_API_CONNECTIONS=255"
    ${PCL_DEFINITIONS}
)

set_target_properties(are-client PROPERTIES
    CXX_STANDARD 14
    # COMPILE_FLAGS "-w"
)

if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(are-client PUBLIC "__WINDOWS__" "_USE_MATH_DEFINES" "DO_NOT_USE_SHARED_MEMORY")
elseif(APPLE)
    target_compile_definitions(are-client PUBLIC "__APPLE__" "DO_NOT_USE_SHARED_MEMORY")
elseif(UNIX)
    target_compile_definitions(are-client PUBLIC "__linux")
    target_link_libraries(are-client rt)
endif()

install(TARGETS are-client DESTINATION bin)
