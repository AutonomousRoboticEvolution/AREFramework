# Locate libraries and headers

set (CMAKE_CXX_STANDARD 14)

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
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/control/ANN.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/control/Control.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/RandNum.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/Settings.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/Neuron.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/InputNeuron.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/OutputNeuron.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/CPGNeuron.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/FunctionNeuron.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/PatternNeuron.cpp"
		"${PROJECT_SOURCE_DIR}/WP4_Ecosytem_Manager/Cplusplus_Evolution/ERFiles/neuron/SimpleNeuron.cpp"
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
