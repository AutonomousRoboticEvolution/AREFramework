#include "physicalER/pi/are_control.hpp"

using namespace are::pi;

AREControl::AREControl(const phy::NN2Individual &ind , std::string stringListOfOrgans, settings::ParametersMapPtr parameters){
    controller = ind;
    _max_eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value * 1000.0; // in milliseconds
    _time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value * 1000.0; // in milliseconds

    // need to turn on the daughter boards
    daughterBoards->init();
    daughterBoards->turnOn();

    // each organ needs to be initiated with its i2c address, obtained from stringListOfOrgans

    std::string thisLine;
    std::stringstream temp_string_stream(stringListOfOrgans);
    if(VERBOSE_DEBUG_PRINTING_AT_SETUP){
        std::cout<<"in AREControl, stringListOfOrgans: "<<stringListOfOrgans<<std::endl;
        std::cout<< "starting main loop:"<< std::endl;
    }
    int NumberOfOrgans = 0;
    while( std::getline(temp_string_stream, thisLine,'\n') ){
        std::string organType = thisLine.substr(0, thisLine.find(","));
        std::string addressValue = thisLine.substr(thisLine.find(",")+1);
        if (organType=="0") {} //Head
        if (organType=="1"){//wheel
            if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"Adding wheel to list, address is "<<addressValue<<std::endl;
            listOfOrgans.push_back( new MotorOrgan( std::stoi(addressValue) ) ); // add a new wheel to the list, with the i2c address just extracted from the line
            NumberOfOrgans++;
        }
        if (organType=="2") { //sensor
            if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"Adding sensor to list, address is "<<addressValue<<std::endl;
            listOfOrgans.push_back( new SensorOrgan( std::stoi(addressValue) ) ); // add a new wheel to the list, with the i2c address just extracted from the line
            NumberOfOrgans++;
        }
    }

    //loop through each organ and work out which daughter board it is connected to:
    if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"Finding daughterboards for each organ..."<<std::endl;
    for (auto thisOrgan : listOfOrgans) {
        daughterBoards->turnOn(LEFT);
        if (thisOrgan->testConnection()){
            // is on LEFT
            if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"organ on LEFT daughter board"<<std::endl;
            thisOrgan->daughterBoardToEnable=LEFT;
        }else{
            daughterBoards->turnOn(RIGHT);
            if (thisOrgan->testConnection()){
                //is on RIGHT
                if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"organ on RIGHT daughter board"<<std::endl;
                thisOrgan->daughterBoardToEnable=RIGHT;
            } else{
                // is not on either - this is bad!
                if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"WARNING cannot find organ on either daughter board"<<std::endl;
                thisOrgan->daughterBoardToEnable=NONE;
            }
        }
    }

    // initialise the LED and flash green to show ready
    ledDriver.reset(new LedDriver(0x6A)); // <- the Led driver is always the same i2c address, it cannot be changed
    ledDriver->init();
    ledDriver->flash();
}

// For each ouput from the controller, send the required value to the low-level wheel object
void AREControl::sendMotorCommands(std::vector<double> values){
    // for each wheel organ, set it's output. The listOfWheels is in the same order as the relevent NN outputs.
    int i=0;
    //for (std::list<Organ>::iterator thisOrgan = listOfWheels.begin(); thisOrgan != listOfWheels.end(); ++thisOrgan){
    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType = WHEEL) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            MotorOrgan* thisWheel = static_cast<MotorOrgan *>(thisOrgan);
            thisWheel->setSpeedNormalised( values[i]);
            i++;
        }
    }

    if(DEBUGGING_INPUT_OUTPUT_DISPLAY){
        // debugging: display sensor values as bars:
        for(i=0;i<values.size();i++){
            int n_blocks=values[i]*5.0 + 5;
            for (int i_block=0; i_block<10;i_block++){
                if (i_block<n_blocks) std::cout<<"o";
                else std::cout<<" ";
            }
        }
        std::cout<<std::endl;
    }
}

void AREControl::retrieveSensorValues(std::vector<double> &sensor_vals){
    // loop through each sensor and get it's value
    sensor_vals.clear();
    sensor_vals = {};
    //for (std::list<SensorOrgan>::iterator thisSensor = listOfSensors.begin(); thisSensor != listOfSensors.end(); ++thisSensor){
    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType = SENSOR) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            SensorOrgan* thisSensor = static_cast<SensorOrgan *>(thisOrgan);
            sensor_vals.push_back( thisSensor->readTimeOfFlightNormalised() );
        }
    }
    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType = SENSOR) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            SensorOrgan* thisSensor = static_cast<SensorOrgan *>(thisOrgan);
            sensor_vals.push_back( thisSensor->readInfraredNormalised() );
        }
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
    std::cout<<"-----|";
}

int AREControl::exec(zmq::socket_t& socket){
    //QCoreApplication qapp(argc,argv);

    std::vector<double> sensor_values;
    std::vector<double> nn_outputs;

    // set up timing system
    uint32_t start_time = millis();
    uint32_t this_loop_start_time = start_time;

    while(this_loop_start_time <= _max_eval_time){

        retrieveSensorValues(sensor_values);

        // tell the controller to update
        controller.set_inputs(sensor_values);
        controller.update ( this_loop_start_time*1000.0 ); //expects time in microseconds, I think? (Matt)
        nn_outputs = controller.get_ouputs();

        // send the new values to the actuators
        sendMotorCommands(nn_outputs);

        // the are-update running on the PC expects to get a message on every timestep:
        zmq::message_t message(40);
        strcpy(static_cast<char*>(message.data()),"pi busy");
        socket.send(message);

        // update timestep value ready for next loop
        this_loop_start_time+=_time_step; // increment
        //std::cout<<"Time now: "<<this_loop_start_time<<std::endl;
        if (millis() > this_loop_start_time){
            // already too late for next loop, so we are falling behind!
            std::cout<<"WARNING: main are_control loop cannot keep up"<<std::endl;
            this_loop_start_time=millis(); // so we don't get further behind
        }else{
            // wait for next loop start time
            while( millis() < this_loop_start_time) { // this is now actaully the target start time of the next loop
                delayMicroseconds(1);
            }
        }

    }

    // turn everything off
    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType = WHEEL) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            MotorOrgan* thisWheel = static_cast<MotorOrgan *>(thisOrgan);
            thisWheel->standby();
        }
    }
    daughterBoards->turnOff();


    // send finished message
    zmq::message_t message(40);
    strcpy(static_cast<char*>(message.data()),"pi finish");
    socket.send(message);

    return 0;
}
