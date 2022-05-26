#include "physicalER/pi/are_control.hpp"
#include <cmath>

using namespace are::pi;

AREControl::AREControl(const phy::NN2Individual &ind , std::string stringListOfOrgans, settings::ParametersMapPtr parameters){
    controller = ind;
    _max_eval_time = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value * 1000.0; // in milliseconds
    _time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value * 1000.0; // in milliseconds
    std::cout<<"Target timestep: "<<_time_step<<" ms"<<std::endl;

    // initilise the camera
    // If this is true, the camera input (binary on/off) will be used the first input to the neural network controller:
    cameraInputToNN =  settings::getParameter<settings::Boolean>(parameters,"#useArucoAsInput").value;
    // This is the parameter which determines which of the arcuo tags trigger the camera:
    camera.setTagsToLookFor( settings::getParameter<settings::Sequence<int>>(parameters,"#arucoTagsToDetect").value );

    // need to turn on the daughter boards
    daughterBoards->init();
    daughterBoards->turnOn();

    // each organ needs to be initiated with its i2c address, obtained from stringListOfOrgans

    // some preliminaries:
    std::string thisLine;
    std::stringstream temp_string_stream(stringListOfOrgans);
    if(VERBOSE_DEBUG_PRINTING_AT_SETUP) std::cout<<"in AREControl, stringListOfOrgans: "<<stringListOfOrgans<<std::endl;
    uint8_t proximal_joint_current_limit = settings::getParameter<settings::Integer>(parameters,"#proximalJointCurrentLimitTensOfMilliAmps").value;
    uint8_t distal_joint_current_limit = settings::getParameter<settings::Integer>(parameters,"#distalJointCurrentLimitTensOfMilliAmps").value;
    uint8_t wheel_current_limit = settings::getParameter<settings::Integer>(parameters,"#wheelOrganCurrentLimitTensOfMilliAmps").value;

    while( std::getline(temp_string_stream, thisLine,'\n') ){
        std::string organType = thisLine.substr(0, thisLine.find(","));
        // NOTE! the string of the address value in the string (from the list_of_organs file) is in decimal, e.g. 0x63 = "99"
        std::string addressValue = thisLine.substr(thisLine.find(",")+1);
        switch (stoi(organType)) {
        case 0: //head
            //do nothing
            break;

        case 1: //wheel
            if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"Adding wheel to list, address is "<<addressValue<<std::endl;
            listOfOrgans.push_back( new MotorOrgan( std::stoi(addressValue) ) ); // add a new wheel to the list, with the i2c address just extracted from the line
            static_cast<MotorOrgan*> (listOfOrgans.back()) ->setCurrentLimit( wheel_current_limit ); // set the current limit
            number_of_wheels++;
            break;

        case 2: //sensor
            if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"Adding sensor to list, address is "<<addressValue<<std::endl;
            listOfOrgans.push_back( new SensorOrgan( std::stoi(addressValue) ) ); // add a new sensor to the list, with the i2c address just extracted from the line
            number_of_sensors++;
            break;

        case 3: //leg - made up of two joints
            if(VERBOSE_DEBUG_PRINTING_AT_SETUP)std::cout<<"Setting up a leg, addresses: "<<addressValue <<" and "<< std::stoi(addressValue)+1 <<std::endl;

            // proximal joint of this leg:
            listOfOrgans.push_back( new JointOrgan( std::stoi(addressValue) ) ); // add a new joint to the list, with the i2c address just extracted from the line
            static_cast<JointOrgan*> (listOfOrgans.back()) ->setCurrentLimit( proximal_joint_current_limit ); // set the curre

            // distal joint of this leg:
            listOfOrgans.push_back( new JointOrgan( std::stoi(addressValue)+1 ) ); // distal joint i2c address is assumed to be one more than that of the proximal joint
            static_cast<JointOrgan*> (listOfOrgans.back()) ->setCurrentLimit( distal_joint_current_limit ); // set the current limit

            number_of_joints+=2;
            break;

        case 4: //caster
            //do nothing
            break;

        default:// shouldn't be here!
            std::cerr << "ERROR! the list of organs contains an entry of uknown type: " <<organType<< std::endl;
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

    // initialise the Head LEDs
    ledDriver.reset(new LedDriver(0x6A)); // <- the Led driver is always the same i2c address, it cannot be changed
    ledDriver->init();

    // battery voltage check
    batteryMonitor.init();
    uint16_t measuredBatteryVoltage = batteryMonitor.measureBatteryVoltage(); // measured in cV
    std::cout<<"Measured battery voltage = "<< float(measuredBatteryVoltage)/100.<<" V"<<std::endl;
    if(measuredBatteryVoltage < 580){
        std::cout<<"#########################\n";
        std::cout<<"#######  WARNING  #######\n";
        std::cout<<"####  BATTERY: "<<float(measuredBatteryVoltage)/100.<<"V ####\n";
        std::cout<<"#########################"<<std::endl;
        ledDriver->flash(RED,1000000,30); // Red for 1 second
    }else{
        ledDriver->flash(GREEN); // flash green to show ready
    }


    // set debug printing flag - this prints a representation of the inputs and outputs to terminal
    if (settings::getParameter<settings::Boolean>(parameters,"#debugDisplayOnPi").value){
        debugDisplayOnPi = true;
    }
    // get the Head LEDs ready for the debugging outputs, if necessary
    if (settings::getParameter<settings::Boolean>(parameters,"#debugLEDsOnPi").value){
        debugLEDsOnPi=true;
    }

    if(VERBOSE_DEBUG_PRINTING_AT_SETUP) std::cout<< "starting main loop:"<< std::endl;
}

// For each ouput from the controller, send the required value to the low-level wheel object
void AREControl::sendOutputOrganCommands(std::vector<double> &values, uint32_t time_milli){
    // for each wheel or joint organ, set it's output. The listOfOrgans is in the same order as the relevent NN outputs.
    int i=0; // index of the organ being considered (in listOfOrgans)
    //for (std::list<Organ>::iterator thisOrgan = listOfWheels.begin(); thisOrgan != listOfWheels.end(); ++thisOrgan){
    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType == WHEEL) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            MotorOrgan* thisWheel = static_cast<MotorOrgan *>(thisOrgan);
            thisWheel->setSpeedNormalised( values[i]);
            logs_to_send<< thisWheel->readMeasuredCurrent()*10 <<","; //add measured current to log
            i++; // increment organ in listOfOrgans
        }
        if (thisOrgan->organType == JOINT) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            JointOrgan* thisJoint = static_cast<JointOrgan *>(thisOrgan);
            double value = values[i];
            value = std::sin(value*(time_millis/1000.0));
            thisJoint->setTargetAngleNormalised(value);
            logs_to_send<< thisJoint->readMeasuredCurrent()*10 << ","; //add measured current to log
            i++; // increment organ in listOfOrgans
        }
        // any other thisOrgan->organType value can be ignored, since it's not an output
    }

    if(debugDisplayOnPi){
        // debugging: display output values as bars:
        for(i=0;i<values.size();i++){
            int n_blocks=values[i]*5.0 + 5; // zero blocks is -1, ten blocks is +1 (so 5 blocks is ~0)
            for (int i_block=0; i_block<10;i_block++){
                if (i_block<n_blocks) std::cout<<"o";
                else std::cout<<" ";
            }
        }
        std::cout<<std::endl;
    }

    // add to log:
    std::copy(values.begin(), values.end(), std::ostream_iterator<int>(logs_to_send, ","));
}

void AREControl::retrieveSensorValues(std::vector<double> &sensor_vals){
    // loop through each sensor and get it's time-of-flight value, then loop through again for the IR values
    sensor_vals.clear();
    sensor_vals = {};

    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType == SENSOR) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            SensorOrgan* thisSensor = static_cast<SensorOrgan *>(thisOrgan);
            sensor_vals.push_back( thisSensor->readTimeOfFlightNormalised() );
        }
    }
    for (auto thisOrgan : listOfOrgans) {
        if (thisOrgan->organType == SENSOR) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            SensorOrgan* thisSensor = static_cast<SensorOrgan *>(thisOrgan);
            sensor_vals.push_back( thisSensor->readInfraredNormalised() );
        }
    }

    // binary camera input
    if (cameraInputToNN){
        if (camera.presenceDetect()) {
            sensor_vals.push_back(1);
        }else{
            sensor_vals.push_back(0);
        }
    }

    if(debugDisplayOnPi){
        // debugging: display sensor values as bars:
        for(int i=0;i<sensor_vals.size();i++){
            int n_blocks=sensor_vals[i]*10.0;
            for (int i_block=0; i_block<10;i_block++){
                if (i_block<n_blocks) std::cout<<"x";
                else std::cout<<" ";
            }
            std::cout<<"|";
        }
        std::cout<<"-----|"; // block between the input and output values to aid readability
    }

    // append input values to log data, as a string:
    std::copy(sensor_vals.begin(), sensor_vals.end(), std::ostream_iterator<int>(logs_to_send, ","));
}

void AREControl::setLedDebugging(std::vector<double> &nn_inputs,std::vector<double> &nn_outputs){
    if (!cameraInputToNN) return; // without aruco tag input, this debugging LED output wouldn't make any sense
    // compute brightness between 10 and 100 (in theory can be 0-255, but the differences are not noticable near the extremes)
    if (nn_inputs.back()>0){ // camera reading is the last NN input
        ledDriver->setAllTo(GREEN,200);
    }else{
        ledDriver->setAllTo(OFF,0);
    }
}

int AREControl::exec(zmq::socket_t& socket){
    //QCoreApplication qapp(argc,argv);

    std::vector<double> sensor_values;
    std::vector<double> nn_outputs;

    // set up timing system
    uint32_t start_time = millis();
    uint32_t this_loop_start_time = start_time;

    // make the first line of the log file, a list of headers for the data to follow:
    logs_to_send<<"time (ms),";
    for(int i=0;i<number_of_sensors;i++){logs_to_send<<"NN_input_TOF_"<<i<<",NN_input_IR_"<<i<<",";}
    if(cameraInputToNN){logs_to_send<<"NN_input_camera,";}
    for(int i=0;i<(number_of_wheels+number_of_joints);i++){logs_to_send<<"current_for_output_"<<i<<"(mA),";}

    while(this_loop_start_time-start_time <= _max_eval_time){
        // start a new line of log file, and add current time
        logs_to_send<<"\n"<<this_loop_start_time-start_time<<",";

        // get sensor readings
        retrieveSensorValues(sensor_values);

        // tell the controller to update
        controller.set_inputs(sensor_values);
        controller.update ( this_loop_start_time*1000.0 ); //expects time in microseconds, I think? (Matt)
        nn_outputs = controller.get_ouputs();

        // send the new values to the actuators
        sendOutputOrganCommands(nn_outputs, this_loop_start_time);

        // set the LED brightnesses for human visualisation (for debugging) - note you need to add #debugDisplayOnPi,bool,1 to the parameters file
        if (debugLEDsOnPi){setLedDebugging(sensor_values,nn_outputs);}

        // the are-update running on the PC expects to get a message on every timestep:
        are::phy::send_string_no_reply("busy",socket,"pi ");

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
        if (thisOrgan->organType = JOINT) {
            daughterBoards->turnOn(thisOrgan->daughterBoardToEnable);
            JointOrgan* thisjoint= static_cast<JointOrgan *>(thisOrgan);
            thisjoint->setServoOff();
        }
    }
    ledDriver->flash(BLUE);
    daughterBoards->turnOff();


    // send finished message
    are::phy::send_string_no_reply("finish",socket,"pi ");


    // send log data
    are::phy::send_string_no_reply( logs_to_send.str() ,socket, "pi ");
    are::phy::send_string_no_reply("finished_logs",socket, "pi ");

    return 0;
}
