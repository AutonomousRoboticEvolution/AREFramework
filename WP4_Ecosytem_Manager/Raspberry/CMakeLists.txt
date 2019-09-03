# Locate libraries and headers

set (CMAKE_CXX_STANDARD 11)

add_executable(robot_controller_are
		main.cpp
		I2CDevice.cpp
		MotorOrgan.cpp
		SensorOrgan.cpp
		Sensors.cpp

		# From VREP
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/control/FixedStructreANN.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/NeuronFactory.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/control/ANN.cpp"
		)

find_package(Threads REQUIRED)

if (OFF)
	# this works
	find_package(WiringPi REQUIRED
			HINTS "${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/cmake"
			)
	target_link_libraries(robot_controller_are WiringPi::WiringPi)
else()
	target_link_libraries(robot_controller_are wiringPi)
endif()

target_link_libraries(robot_controller_are ${CMAKE_THREAD_LIBS_INIT})
