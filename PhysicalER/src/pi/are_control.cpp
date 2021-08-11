#include "physicalER/pi/are_control.hpp"

using namespace are::pi;

AREControl::AREControl(const NN2Individual &ind , std::string stringListOfOrgans, settings::ParametersMapPtr parameters){
    controller = ind;
    _max_eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value * 1000000.0;

    // need to turn on the daughter boards
    daughterBoards->init();
    daughterBoards->turnOn();

    // each organ needs to be initiated with its i2c address, obtained from stringListOfOrgans
    std::cout<<"in AREControl, stringListOfOrgans: "<<stringListOfOrgans<<std::endl;
    std::string thisLine;
    std::stringstream temp_string_stream(stringListOfOrgans);
    std::cout<< "starting main loop:"<< std::endl;
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

    //loop through each sensor and work out which daughter board it is connected to:
    std::cout<<"Finding daughterboards for each sensor..."<<std::endl;
    for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
        daughterBoards->turnOn(LEFT);
        if (thisSensor->test()){
            // is on LEFT
            std::cout<<"organ on LEFT daughter board"<<std::endl;
            thisSensor->daughterBoardToEnable=LEFT;
        }else{
            daughterBoards->turnOn(RIGHT);
            if (thisSensor->test()){
                //is on RIGHT
                std::cout<<"organ on RIGHT daughter board"<<std::endl;
                thisSensor->daughterBoardToEnable=RIGHT;
            } else{
                // is not on either - this is bad!
                std::cout<<"WARNING cannot find organ on either daughter board"<<std::endl;
            }
        }
    }

    // initialise the LED and flash green to show ready
    ledDriver.reset(new LedDriver(0x6A)); // <- the Led driver is always the same i2c address, it cannot be cahnged
    ledDriver->init();
    ledDriver->flash();
//    ledId leds[4] = {RGB0, RGB1, RGB2, RGB3};
//    for (int i=0; i<4; ++i) { // loop through and turn on all the LEDS
//        ledDriver->setMode(leds[i], PWM, ALL);
//        ledDriver->setBrightness(leds[i], 150);
//        ledDriver->setColour(leds[i],GREEN);
//    }
//    usleep(1000000); // 1 second
//    for (int i=0; i<4; ++i) { // loop through and turn off all the LEDS
//        ledDriver->setMode(leds[i],FULL_OFF,ALL);
//    }
}

// For each ouput from the controller, send the required value to the low-level wheel object
void AREControl::sendMotorCommands(std::vector<double> values){
    int i=0;
    for (std::list<MotorOrgan>::iterator thisWheel = listOfWheels.begin(); thisWheel != listOfWheels.end(); ++thisWheel){
        daughterBoards->turnOn(thisWheel->daughterBoardToEnable);
        thisWheel->setSpeed( values[i]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER );
//        std::cout<<"Wheel "<<i<<" output set to "<<values[i]*NEURAL_NETWORK_OUTPUT_TO_WHEEL_INPUT_MULTIPLIER<<std::endl;
        i++;
    }
}

void AREControl::retrieveSensorValues(std::vector<double> &sensor_vals){
    // loop through each sensor and get it's value
    sensor_vals.clear();
    sensor_vals = {};
    int i=0;
    for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
        daughterBoards->turnOn(thisSensor->daughterBoardToEnable);
        int raw_value = thisSensor->readTimeOfFlight();
        sensor_vals.push_back( std::max ( std::min(1.0,float(raw_value)/1000.0) , 0.0) );
    }
    for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
        daughterBoards->turnOn(thisSensor->daughterBoardToEnable);
        int raw_value = thisSensor->readInfrared();
        if (raw_value > INFRARED_SENSOR_THREASHOLD) sensor_vals.push_back(1.0);
        else sensor_vals.push_back(0.0);
    }

    // debugging: display sensor values as bars:
    for(i=0;i<sensor_vals.size();i++){
        int n_blocks=sensor_vals[i]*10.0;
        for (int i_block=0; i_block<10;i_block++){
            if (i_block<n_blocks) std::cout<<"x";
            else std::cout<<" ";
        }
        std::cout<<"|";
    }
    std::cout<<std::endl;
}

int AREControl::exec(zmq::socket_t& socket){
    //QCoreApplication qapp(argc,argv);

    std::vector<double> sensor_values;
    std::vector<double> nn_outputs;
    double time = 0;
    std::cout<<"evaluation time: "<<_max_eval_time<<std::endl;
    while(time <= _max_eval_time){

        retrieveSensorValues(sensor_values);

        // tell the controller to update
        controller.set_inputs(sensor_values);
        controller.update(time);
        nn_outputs = controller.get_ouputs();
        
        // send the new values to the actuators
        sendMotorCommands(nn_outputs);
        
        // update timestep
        time+=_time_step;

        // the are-update running on the PC expects to get a message on every timestep:
        zmq::message_t message(40);
        strcpy(static_cast<char*>(message.data()),"pi busy");
        socket.send(message);
        
        // wait for next timestep
//        std::cout<<" === "<<time<<" === "<<std::endl;
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
