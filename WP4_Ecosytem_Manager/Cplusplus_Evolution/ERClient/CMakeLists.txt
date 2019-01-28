cmake_minimum_required(VERSION 3.0)

find_package(Threads REQUIRED)

set(SOURCES
    ClientEA.cpp
    ERClient.cpp
    SlaveConnection.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/../vrep-programming/remoteApi/extApi.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vrep-programming/remoteApi/extApiPlatform.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vrep-programming/common/shared_memory.c"
)

add_executable(ERClient ${SOURCES})
target_include_directories(ERClient PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/../vrep-programming"
    "${CMAKE_CURRENT_SOURCE_DIR}/../vrep-programming/include"
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
