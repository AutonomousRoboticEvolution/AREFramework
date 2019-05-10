
install(FILES DefaultERLight.ttt
        DESTINATION ${VREP_FOLDER}/scenes)

install(FILES
        catModel.ttm
        Servo_Module.ttm
        C_Brain.ttm
        C_Sensor.ttm
        C_Wheel.ttm
        C_Servo.ttm
        brainOrgan.ttm
        gripper.ttm
        motorOrgan.ttm
        sensorOrgan.ttm
        DESTINATION ${VREP_FOLDER}/models)
