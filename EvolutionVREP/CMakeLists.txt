cmake_minimum_required(VERSION 3.0)
project(ER)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (NOT VREP_FOLDER)
    message(FATAL_ERROR "VREP_FOLDER variable not set, please set the variable to the vrep location. Example: -DVREP_FOLDER=/usr/share/vrep/")
else()
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
endif()

find_package(MultiNEAT REQUIRED)
find_package(PolyVox REQUIRED)

FILE(GLOB_RECURSE SOURCES "RandNum.cpp" "ERFiles/*.cpp" "ERFiles/EA_MultiNEAT.h" "${VREP_FOLDER}/programming/common/v_repLib.cpp")

add_library(ER_library SHARED ${SOURCES})

target_include_directories(ER_library PUBLIC
    "${VREP_FOLDER}/programming/include"
)

# tool to help remove the warnings when using GCC
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

set_target_properties(ER_library PROPERTIES 
    CXX_STANDARD 14
#    COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

add_library(v_repExtER SHARED 
    v_repExtER.cpp
)

target_link_libraries(ER_library
    MultiNEAT::MultiNEAT
    dlibxx dl
)

target_include_directories(ER_library
    PRIVATE
    ${PolyVox_INCLUDE_DIRS}
)

install(TARGETS ER_library DESTINATION lib)

target_link_libraries(v_repExtER ER_library dlibxx dl)

target_include_directories(v_repExtER PUBLIC
    "${VREP_FOLDER}/programming/include"
)

set_target_properties(v_repExtER PROPERTIES 
    CXX_STANDARD 14
    # COMPILE_FLAGS "-w"
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS v_repExtER DESTINATION ${VREP_LIB_FOLDER})

if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(ER_library PUBLIC "__WINDOWS__")
elseif(APPLE)
    target_compile_definitions(ER_library PUBLIC "__APPLE__")
elseif(UNIX)
    target_compile_definitions(ER_library PUBLIC "__linux")
endif()

add_subdirectory(ERClient)
add_subdirectory(VREP_Files)

add_subdirectory(examples)
