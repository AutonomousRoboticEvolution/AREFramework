#include "physicalER/pi/are_control.hpp"

using namespace are::pi;

AREControl::AREControl(const NN2Individual &ind){
    controller = ind;

	// need to turn on the daughter boards
	daughterBoards->init();
    daughterBoards->turnOn();

    // each organ needs to be initiated with its i2c address.
    wheel0.reset(new MotorOrgan(0x60));
    wheel1.reset(new MotorOrgan(0x61));
    wheel2.reset(new MotorOrgan(0x62));

    // initialise the LED and flash green to show ready
    ledDriver.reset(new LedDriver(0x6A)); // <- the Led driver is always the same i2c address, it cannot be cahnged
    ledDriver->init();
    ledId leds[4] = {RGB0, RGB1, RGB2, RGB3};
    for (int i=0; i<4; ++i) { // loop through and turn on all the LEDS
        ledDriver->setMode(leds[i], PWM, ALL);
        ledDriver->setBrightness(leds[i], 150);
        ledDriver->setColour(leds[i],GREEN);
    }
    usleep(500000); // 0.5 second
    for (int i=0; i<4; ++i) { // loop through and turn off all the LEDS
        ledDriver->setMode(leds[i],FULL_OFF,ALL);
    }
}

// For each ouput from the controller, send the required value to the low-level wheel object
void AREControl::sendMotorCommands(std::vector<double> values){
    std::cout << "wheel0 setSpeed to: " << values[0]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER << std::endl;
    std::cout << "wheel1 setSpeed to: " << values[1]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER << std::endl;
    std::cout << "wheel2 setSpeed to: " << values[2]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER << std::endl;
    wheel0->setSpeed(values[0]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER);
    wheel1->setSpeed(values[1]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER);
    wheel2->setSpeed(values[2]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER);
}

void AREControl::retrieveSensorValues(std::vector<double> &sensor_vals){
    // Sensors are not yet implemented for ARE robot
    //sensor_vals.clear();
    sensor_vals = {0,0};
}

int AREControl::exec(int argc, char** argv, zmq::socket_t& socket){
    //QCoreApplication qapp(argc,argv);

    std::vector<double> sensor_values;
    std::vector<double> nn_outputs;
    double time = 0;
    while(time <= _max_eval_time){

        retrieveSensorValues(sensor_values);

        // tell the controller to update
        controller.set_inputs(sensor_values);
        controller.update(time);
        nn_outputs = controller.get_ouputs();

        // some debugging messages
        std::cout << "Time now: " << time << std::endl;
        std::cout << "nn_outputs 0: " << nn_outputs[0] << std::endl;
        std::cout << "nn_outputs 1: " << nn_outputs[1] << std::endl;
        std::cout << "nn_outputs 2: " << nn_outputs[2] << std::endl;
        
        // send the new values to the actuators
        sendMotorCommands(nn_outputs);
        
        // update timestep
        time+=_time_step;

        // the are-update running on the PC expects to get a message on every timestep:
        zmq::message_t message(40);
        strcpy(static_cast<char*>(message.data()),"pi busy");
        socket.send(message);
        
        // wait for next timestep
        usleep(_time_step);
    }

    // turn everything off
    wheel0->standby();
    wheel1->standby();
    wheel2->standby();
    daughterBoards->turnOff();

    
    // send finished message
    zmq::message_t message(40);
    strcpy(static_cast<char*>(message.data()),"pi finish");
    socket.send(message);

    return 0;
}
