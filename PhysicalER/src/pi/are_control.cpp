#include "physicalER/pi/are_control.hpp"

using namespace are::pi;

AREControl::AREControl(const NN2Individual &ind){
    controller = ind;
    // each organ needs to be initiated with its i2c address.
    wheel0.reset(new MotorOrgan(0x60));
}

void AREControl::sendMotorCommands(std::vector<double> values){
    wheel0->setSpeed(values[0]*50.f);
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
        
        // send the new values to the actuators
        sendMotorCommands(nn_outputs);
        
        // update timestep
        time+=_time_step;

        // the are-update running on the PC expects to get a message on every timestep:
        zmq::message_t message(40);
        strcpy(static_cast<char*>(message.data()),"pi busy");
        socket.send(message);
        
        // wait for next timestep
        sleep(_time_step);
    }
    
    // send finished message
    zmq::message_t message(40);
    strcpy(static_cast<char*>(message.data()),"pi finish");
    socket.send(message);

    return 0;
}
