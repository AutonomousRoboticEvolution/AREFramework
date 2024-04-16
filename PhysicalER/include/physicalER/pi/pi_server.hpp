#ifndef PI_SERVER_HPP
#define PI_SERVER_HPP

#include "ARE/Genome.h"
#include "ARE/Settings.h"
#include <memory>

namespace are{
namespace pi{

template<class Ctrl>
std::shared_ptr<Ctrl> controller_instanciation(Genome::Ptr &genome, are::settings::ParametersMapPtr &parameters, are::misc::RandNum::Ptr &rand_num){
    EmptyGenome::Ptr empty_gen;
    std::shared_ptr<Ctrl> controller = std::make_shared<Ctrl>(empty_gen,genome);
    controller->set_parameters(parameters);
    controller->
            set_randNum(rand_num);
    controller->init();
    return controller;
}




}//pi
}//are

#endif //PI_SERVER_HPP
