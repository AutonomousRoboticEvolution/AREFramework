cmake_minimum_required(VERSION 3.0)

if(RASPBERRY)

#set(CMAKE_AUTOMOC ON)

#find_package(Qt5 COMPONENTS Core Test DBus REQUIRED)

file(GLOB SRC src/pi/* src/pi_communication.cpp)
set(CTRL_SRC ../RobotController/I2CDevice.cpp ../RobotController/MotorOrgan.cpp ../RobotController/SensorOrgan.cpp ../RobotController/DaughterBoards.cpp ../RobotController/LedDriver.cpp)
add_executable(are-pi-server ${SRC} ${CTRL_SRC})
target_include_directories(are-pi-server PUBLIC
    "include"
    "../EAFramework/include"
    "../RobotController/")
target_link_libraries(are-pi-server piARE zmq  ${Boost_LIBRARIES} wiringPi)#Qt5::Core Qt5::DBus Qt5::Test

else()
find_package(OpenCV REQUIRED)

file(GLOB SRC src/*)

add_library(physicalER SHARED ${SRC} src/pi/pi_individual.cpp)
target_include_directories(physicalER PUBLIC
     ${OpenCV_INCLUDE_DIRS}
    "include/"
     "../EAFramework/include")
target_link_libraries(physicalER ARE dlibxx zmq)

install(TARGETS physicalER DESTINATION lib)
install(DIRECTORY . DESTINATION include/PhysicalER FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" )

add_executable(are-generate src/app/generate.cpp)
target_include_directories(are-generate PUBLIC
     ${OpenCV_INCLUDE_DIRS}
    "include/"
     "../EAFramework/include")
target_link_libraries(are-generate ARE physicalER)

add_executable(are-update src/app/update.cpp)
target_include_directories(are-update PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "include/"
     "../EAFramework/include")
target_link_libraries(are-update ARE physicalER)

install(TARGETS are-generate are-update DESTINATION bin)
endif()
