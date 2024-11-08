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

namespace are_sett = are::settings;

int main(int argc, char** argv) {
    //*/  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    zmq::socket_t reply (context, ZMQ_REP);
    publisher.bind ("tcp://*:5555");
    reply.bind ("tcp://*:5556");
    /**/

    settings::ParametersMapPtr parameters;
    
    misc::RandNum::Ptr randomNumber = std::make_shared<misc::RandNum>(0);

    EmptyGenome::Ptr empy_gen = std::make_shared<EmptyGenome>();
    NNParamGenome::Ptr ctrl_gen = std::make_shared<NNParamGenome>();

    std::string str_ctrl, str_organs_list, str_param;

    while(1){
        //receive parameters
        phy::receive_string(str_param,"parameters_received",reply,"pi ");
        std::cout<<"Parameters received:\n"<<str_param<<std::endl;
    	parameters = std::make_shared<settings::ParametersMap>(settings::fromString(str_param));
        //receive organ addresses list
        phy::receive_string(str_organs_list,"organ_addresses_received",reply,"pi ");
        std::cout<<"Organs list received: \n"<<str_organs_list<<std::endl;
        

        // this generates the neural network controller ind
        phy::receive_string(str_ctrl,"starting",reply,"pi ");
        std::cout<<"NN Genome as string:\n"<<str_ctrl<<std::endl;
        str_ctrl.erase(0, str_ctrl.find("\n") + 1);
	    ctrl_gen->from_string(str_ctrl);
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

