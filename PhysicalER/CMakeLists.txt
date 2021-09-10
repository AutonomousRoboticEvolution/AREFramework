cmake_minimum_required(VERSION 3.0)

if(RASPBERRY)
file(GLOB SRC src/pi/* src/pi_communication.cpp src/pi_individual.cpp)
set(CTRL_SRC ../RobotController/I2CDevice.cpp ../RobotController/Organ.cpp ../RobotController/MotorOrgan.cpp ../RobotController/SensorOrgan.cpp ../RobotController/DaughterBoards.cpp ../RobotController/LedDriver.cpp)
add_executable(are-pi-server ${SRC} ${CTRL_SRC})
target_include_directories(are-pi-server PUBLIC
    "include"
    "../EAFramework/include"
    "../RobotController/")
target_link_libraries(are-pi-server piARE zmq  ${Boost_LIBRARIES} wiringPi)

else()
#find_package(OpenCV REQUIRED)

#Compilation of update

file(GLOB SRC_UPDATE src/update/*)

add_executable(are-update ${SRC_UPDATE} src/pi_communication.cpp src/pi_individual.cpp)
target_include_directories(are-update PUBLIC
    # ${OpenCV_INCLUDE_DIRS}
    "include/"
     "../EAFramework/include")
target_link_libraries(are-update ARE dlibxx dl zmq)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#Compilation of generate

# CoppeliaSim stuff

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
    message("Using VREP is deprecated")
else()
    message(FATAL_ERROR "(should never reach here) VREP_FOLDER or COPPELIASIM_FOLDER variables not set, please set the variable to the vrep/coppeliasim location. Example: -DVREP_FOLDER=/usr/share/vrep/ or -DCOPPELIASIM_FOLDER=/usr/share/coppeliasim")
endif()
find_package(PolyVox REQUIRED)
find_package(MultiNEAT REQUIRED)

SET(VREP_SRC
        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/simLib.cpp>
        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApi.c>
        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/remoteApi/extApiPlatform.c>
        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/common/shared_memory.c>
        )

file(GLOB SRC_GENERATE src/generate/*)

if(VREP_FOUND)
    message("Using VREP is deprecated")
elseif(COPPELIASIM_FOUND)
    SET(LIB_EXT simExtGenerate)
    SET(PLUGIN_SRC ${SRC_GENERATE} src/pi_individual.cpp)
endif()

add_library(${LIB_EXT} SHARED ${VREP_SRC} ${PLUGIN_SRC})

target_include_directories(${LIB_EXT} PUBLIC
        "../EAFramework/include"
        "include"
        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
        )


target_link_libraries(${LIB_EXT}
        dlibxx dl rt
        ${Boost_LIBRARIES}
        ARE simulatedER
        )

target_compile_definitions(${LIB_EXT} PUBLIC
        "MAX_EXT_API_CONNECTIONS=255"
        "NON_MATLAB_PARSING"
        )
set_target_properties(${LIB_EXT} PROPERTIES
        CXX_STANDARD 14
        ##    COMPILE_FLAGS "-w"
        POSITION_INDEPENDENT_CODE ON
        )
if (WIN32) #if(MSVC OR MSYS OR MINGW) for detecting specific compilers
    target_compile_definitions(${LIB_EXT} PUBLIC "__WINDOWS__")
elseif(APPLE)
    target_compile_definitions(${LIB_EXT} PUBLIC "__APPLE__")
elseif(UNIX)
    target_compile_definitions(${LIB_EXT} PUBLIC "__linux")
endif()


if (COPPELIASIM_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${COPPELIASIM_FOLDER})
elseif (VREP_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${VREP_FOLDER})
endif()
install(TARGETS are-update DESTINATION bin)
install(DIRECTORY . DESTINATION include/PhysicalER FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

#install(TARGETS are-generate are-update DESTINATION bin)
endif()
