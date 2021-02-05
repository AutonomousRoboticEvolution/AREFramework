#include "physicalER/pi/pimio_control.hpp"

using namespace are::pi;

PimioControl::PimioControl(const NN2Individual &ind){
    controller = ind;

    // interface instantiation
    std::cout << "\n-attempting DBus connection-" << std::endl;
    interface = new Aseba::DBusInterface();

    // load an Aseba script onto Thymio II
    std::cout << "\n-loading an aesl script onto the robot-" << std::endl;
    interface->loadScript("ScriptDBusThymio.aesl");

}

void PimioControl::sendMotorCommand(double left,double right){
    interface->setVariable("thymio-II", "motor.left.target", Values({left}));
    interface->setVariable("thymio-II", "motor.right.target", Values({right}));
}

void PimioControl::retrieveSensorValues(std::vector<double> &sensor_vals){
    QList<short> prox_val = interface->getVariable("thymio-II","prox.horizontal");
    sensor_vals.clear();
    for(const short &v : prox_val)
        sensor_vals.push_back(v);
}

int PimioControl::exec(int argc, char** argv, zmq::socket_t& socket){
    QCoreApplication qapp(argc,argv);

    std::vector<double> sensor_values;
    std::vector<double> nn_outputs;
    double time = 0;
    while(time <= _max_eval_time){

        retrieveSensorValues(sensor_values);
        controller.set_inputs(sensor_values);
        controller.update(time);
        nn_outputs = controller.get_ouputs();
        sendMotorCommand(nn_outputs[0]*50,nn_outputs[1]*50);
        
        time+=_time_step;

        zmq::message_t message(40);
        strcpy(static_cast<char*>(message.data()),"pi busy");
        socket.send(message);
        
        QTest::qSleep(_time_step);
    }

    zmq::message_t message(40);
    strcpy(static_cast<char*>(message.data()),"pi finish");
    socket.send(message);

    return 0;
}
