#include "physicalER/pi/are_control.hpp"

using namespace are::pi;

AREControl::AREControl(const NN2Individual &ind , std::string stringListOfOrgans){
    controller = ind;

    // need to turn on the daughter boards
    daughterBoards->init();
    daughterBoards->turnOn();

    // each organ needs to be initiated with its i2c address, obtained from stringListOfOrgans
    std::cout<<"in AREControl, stringListOfOrgans: "<<stringListOfOrgans<<std::endl;
    std::string thisLine;
    std::stringstream temp_string_stream(stringListOfOrgans);
    std::cout<< "looping:"<< std::endl;
    while( std::getline(temp_string_stream, thisLine,'\n') ){
        std::string organType = thisLine.substr(0, thisLine.find(","));
        std::string addressValue = thisLine.substr(thisLine.find(",")+1);
        if (organType=="0") {} //Head
        if (organType=="1"){//wheel
            std::cout<<"Adding wheel to list, address is "<<addressValue<<std::endl;
            listOfWheels.push_back( MotorOrgan( std::stoi(addressValue) ) ); // add a new wheel to the list, with the i2c address just extracted from the line
        }
        if (organType=="2") { //sensor
            std::cout<<"Adding sensor to list, address is "<<addressValue<<std::endl;
            listOfSensors.push_back( SensorOrgan( std::stoi(addressValue) ) ); // add a new wheel to the list, with the i2c address just extracted from the line
        }
    }

    // initialise the LED and flash green to show ready
    ledDriver.reset(new LedDriver(0x6A)); // <- the Led driver is always the same i2c address, it cannot be cahnged
    ledDriver->init();
    ledId leds[4] = {RGB0, RGB1, RGB2, RGB3};
    for (int i=0; i<4; ++i) { // loop through and turn on all the LEDS
        ledDriver->setMode(leds[i], PWM, ALL);
        ledDriver->setBrightness(leds[i], 150);
        ledDriver->setColour(leds[i],GREEN);
    }
    usleep(1000000); // 1 second
    for (int i=0; i<4; ++i) { // loop through and turn off all the LEDS
        ledDriver->setMode(leds[i],FULL_OFF,ALL);
    }
}

// For each ouput from the controller, send the required value to the low-level wheel object
void AREControl::sendMotorCommands(std::vector<double> values){
    for (int i=0; i<listOfWheels.size();i++){
        std::cout<<i<<std::endl;
    }
//    int i=0;
//    for (std::list<MotorOrgan>::iterator thisWheel = listOfWheels.begin(); thisWheel != listOfWheels.end(); ++thisWheel){
//        thisWheel->setSpeed( values[i]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER );
//        std::cout<<"Wheel "<<i<<" output set to "<<values[i]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER<<std::endl;
//        i++;
//    }
}

void AREControl::retrieveSensorValues(std::vector<double> &sensor_vals){
    // loop through each sensor and get it's value
    sensor_vals.clear();
    sensor_vals = {};
    for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
        sensor_vals.push_back( thisSensor->readTimeOfFlight() );
    }
}

int AREControl::exec(zmq::socket_t& socket){
    //QCoreApplication qapp(argc,argv);

    std::vector<double> sensor_values;
    std::vector<double> nn_outputs;
    double time = 0;
    std::cout << "time,nn_output 0,nn_output 1,nn_output 2,nn_output 3"<< std::endl;
    while(time <= _max_eval_time){

        retrieveSensorValues(sensor_values);

        // tell the controller to update
        controller.set_inputs(sensor_values);
        controller.update(time);
        nn_outputs = controller.get_ouputs();

        // some debugging messages
        std::cout << float(time)/1000000 <<","<< nn_outputs[0] <<","<< nn_outputs[1] <<","<< nn_outputs[2] <<","<< nn_outputs[3] <<","<< std::endl;
        //std::cout << "nn_outputs : " << nn_outputs[0] << std::endl;
        //std::cout << "nn_outputs 1: " << nn_outputs[1] << std::endl;
        //std::cout << "nn_outputs 2: " << nn_outputs[2] << std::endl;
        //std::cout << "nn_outputs 3: " << nn_outputs[3] << std::endl;
        
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
    for (std::list<MotorOrgan>::iterator thisWheel = listOfWheels.begin(); thisWheel != listOfWheels.end(); ++thisWheel){
        thisWheel->standby() ;
    }
    daughterBoards->turnOff();

    
    // send finished message
    zmq::message_t message(40);
    strcpy(static_cast<char*>(message.data()),"pi finish");
    socket.send(message);

    return 0;
}
