cmake_minimum_required(VERSION 3.0)

if(RASPBERRY)

#set(CMAKE_AUTOMOC ON)

#find_package(Qt5 COMPONENTS Core Test DBus REQUIRED)

file(GLOB SRC src/pi/* src/pi_communication.cpp)
set(CTRL_SRC ../RobotController/I2CDevice.cpp ../RobotController/MotorOrgan.cpp ../RobotController/SensorOrgan.cpp ../RobotController/DaughterBoards.cpp ../RobotController/LedDriver.cpp ../RobotController/Sensors.cpp)
add_executable(are-pi-server ${SRC} ${CTRL_SRC})
target_include_directories(are-pi-server PUBLIC
    "include"
    "../EAFramework/include"
    "../RobotController/")
target_link_libraries(are-pi-server piARE zmq  ${Boost_LIBRARIES} wiringPi)#Qt5::Core Qt5::DBus Qt5::Test

else()
#find_package(OpenCV REQUIRED)

file(GLOB SRC src/*)

add_library(physicalER SHARED ${SRC} src/pi/pi_individual.cpp)
target_include_directories(physicalER PUBLIC
    # ${OpenCV_INCLUDE_DIRS}
    "include/"
     "../EAFramework/include")
target_link_libraries(physicalER ARE dlibxx zmq)

install(TARGETS physicalER DESTINATION lib)
install(DIRECTORY . DESTINATION include/PhysicalER FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

add_executable(are-update src/app/update.cpp)
target_include_directories(are-update PUBLIC
    #${OpenCV_INCLUDE_DIRS}
    "include/"
     "../EAFramework/include")
target_link_libraries(are-update ARE physicalER)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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
find_package(MultiNEAT REQUIRED)

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

#add_library(simulatedER SHARED ${VREP_SRC} ${SRC})

#target_include_directories(simulatedER PUBLIC
#        $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
#        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
#        "include"
#        "ERClient/include"
#        "../EAFramework/include"
#        )

if(VREP_FOUND)
    SET(LIB_EXT v_repExtERGenerate)
    SET(PLUGIN_SRC src/app/v_repExtER.cpp)
elseif(COPPELIASIM_FOUND)
    #SET(LIB_EXT simExtER)
    SET(LIB_EXT simExtGenerate)
    SET(PLUGIN_SRC src/app/simExtGenerate.cpp)
endif()

add_library(${LIB_EXT} SHARED ${VREP_SRC} ${PLUGIN_SRC})

target_include_directories(${LIB_EXT} PUBLIC
#        "include"
#        "ERClient/include"
        "../EAFramework/include"
        "include"
        $<$<BOOL:${VREP_FOUND}>:${VREP_FOLDER}/programming/include>
        $<$<BOOL:${COPPELIASIM_FOUND}>:${COPPELIASIM_FOLDER}/programming/include>
        )

if (COPPELIASIM_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${COPPELIASIM_FOLDER})
elseif (VREP_FOUND)
    install(TARGETS ${LIB_EXT} DESTINATION ${VREP_FOLDER})
endif()

target_link_libraries(${LIB_EXT}
        dlibxx dl rt
        ${Boost_LIBRARIES}
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

#add_executable(are-generate src/app/generate.cpp)
target_include_directories(simExtGenerate PUBLIC
        "include/"
        "../EAFramework/include")
target_link_libraries(simExtGenerate ARE physicalER)
#
#install(TARGETS are-generate are-update DESTINATION bin)
install(TARGETS simExtGenerate are-update DESTINATION bin)
endif()
