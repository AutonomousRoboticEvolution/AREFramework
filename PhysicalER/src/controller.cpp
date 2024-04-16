#include <physicalER/controller.hpp>

using namespace are::phy;

void Controller::update(double delta_time){
   outputs = control->update(inputs);
}


