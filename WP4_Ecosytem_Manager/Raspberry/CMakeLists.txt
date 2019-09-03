# Locate libraries and headers

add_executable(robot_controller_are
		main.cpp
		I2CDevice.cpp
		MotorOrgan.cpp
		SensorOrgan.cpp
		Sensors.cpp
		)

if (OFF)
	# this works
	find_package(WiringPi REQUIRED
			HINTS "${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/cmake"
			)
else()
	target_link_libraries(robot_controller_are wiringPi)
endif()
find_package(Threads REQUIRED)

#target_include_directories(robot_controller_are ${WiringPi_INCLUDE_DIR})

# Link against libraries
target_link_libraries(robot_controller_are
		WiringPi::WiringPi
		${CMAKE_THREAD_LIBS_INIT})
