#include <zmq.hpp>
#include <string>
#include <iostream>

#include "physicalER/pi_individual.hpp"
#include "physicalER/pi_communication.hpp"
#include "physicalER/pi/are_control.hpp"
#include "ARE/Settings.h"
#include "ARE/misc/RandNum.h"

using namespace are;


//TODO implement other orders like abort


int main(int argc, char** argv) {
    //*/  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    zmq::socket_t reply (context, ZMQ_REP);
    publisher.bind ("tcp://*:5555");
    reply.bind ("tcp://*:5556");
    /**/

    settings::ParametersMapPtr parameters;
    
    misc::RandNum::Ptr randomNumber(new misc::RandNum(0));

    EmptyGenome::Ptr empy_gen(new EmptyGenome);
    NNParamGenome::Ptr ctrl_gen(new NNParamGenome);

    std::string str_ctrl, str_organs_list, str_param;

//    // temp!
//    #include <wiringPi.h> //for timing functions
//    Camera camera;
//    while(1){
//        camera.setTagsToLookFor({14});
//        camera.presenceDetect();
//        delay(100);
//    }

    while(1){
        //receive parameters
        phy::receive_string(str_param,"parameters_received",reply);
        std::cout<<"Parameters received:\n"<<str_param<<std::endl;
        parameters = std::make_shared<settings::ParametersMap>(settings::fromString(str_param));
        //receive organ addresses list
        phy::receive_string(str_organs_list,"organ_addresses_received",reply);
        std::cout<<"Organs list received: \n"<<str_organs_list<<std::endl;

        // this generates the neural network controller ind
        phy::receive_string(str_ctrl,"starting",reply);
        ctrl_gen->from_string(str_ctrl);
        std::cout<<"NN Genome as sting:\n"<<str_ctrl<<std::endl;
        phy::NN2Individual ind(empy_gen,ctrl_gen);
        ind.set_parameters(parameters);
        ind.set_randNum(randomNumber);
        ind.init();

        // run controller
        std::cout<<"running a controller"<<std::endl;        
        pi::AREControl AREController(ind, str_organs_list, parameters);
        AREController.exec(publisher);
        std::cout<<"finished running the controller"<<std::endl;

    }



    return 0;
}

