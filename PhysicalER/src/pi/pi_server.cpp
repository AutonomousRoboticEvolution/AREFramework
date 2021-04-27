#include <zmq.hpp>
#include <string>
#include <iostream>

#include "physicalER/pi/pi_individual.hpp"
#include "physicalER/pi/are_control.hpp"
#include "ARE/Settings.h"
#include "ARE/misc/RandNum.h"
using namespace are;


//TODO implement other orders like abort

using are_sett = are::setting;

int main(int argc, char** argv) {
    //*/  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t publisher (context, ZMQ_PUB);
    zmq::socket_t reply (context, ZMQ_REP);
    publisher.bind ("tcp://*:5555");
    reply.bind ("tcp://*:5556");
    /**/

    //*/ Load Parameter file. Change to be retrieve through communication
    settings::ParametersMapPtr parameters = std::make_shared<settings::ParametersMap>(
                settings::loadParameters(argv[1]));
    /**/
    
    misc::RandNum::Ptr randomNumber(new misc::RandNum(0));

    zmq::message_t message("");
    EmptyGenome::Ptr empy_gen(new EmptyGenome);
    NNParamGenome::Ptr ctrl_gen(new NNParamGenome);

    while(1){
        zmq::message_t reply_msg;
        strcpy(static_cast<char*>(reply_msg.data()),"starting");
        reply.recv(&message);
        reply.send(reply_msg);
        
        // this generates the neural network controller ind
        std::string str_ctrl(static_cast<char*>(message.data()));
        str_ctrl.erase(0,str_ctrl.find(' ')+1);
        std::cout << str_ctrl << std::endl;
        ctrl_gen->from_string(str_ctrl);
        pi::NN2Individual ind(empy_gen,ctrl_gen);
        ind.set_parameters(parameters);
        ind.set_randNum(randomNumber);
        ind.init();

        std::cout<<"running a controller"<<std::endl;        
        pi::AREControl AREController(ind);
        AREController.exec(argc,argv,publisher);
        std::cout<<"finished running the controller"<<std::endl;
        

    }



    return 0;
}

