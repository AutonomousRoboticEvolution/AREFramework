find_package(OpenCV REQUIRED) # for camera
include_directories( ${OpenCV_INCLUDE_DIRS} )

file(GLOB SRC robot_tester.cpp)

set(CTRL_SRC ./I2CDevice.cpp ./Organ.cpp ./JointOrgan.cpp ./MotorOrgan.cpp ./SensorOrgan.cpp ./DaughterBoards.cpp ./LedDriver.cpp ./Camera.cpp ./BatteryMonitor.cpp ./Fan.cpp)
add_executable(robot_tester ${SRC} ${CTRL_SRC})
target_include_directories(robot_tester PUBLIC
    "./")
target_link_libraries(robot_tester piARE zmq  ${Boost_LIBRARIES} wiringPi ${OpenCV_LIBS})

