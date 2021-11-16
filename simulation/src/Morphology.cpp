#include "simulatedER/Morphology.h"

using namespace are::sim;

std::vector<double> Morphology::update(){
    std::vector<double> sensorValues;
    bool use_simulate_data = settings::getParameter<settings::Boolean>(parameters, "#use_sim_sensor_data").value;
    readProximitySensors(proxHandles,sensorValues);
    readPassivIRSensors(IRHandles, sensorValues, use_simulate_data);

    return sensorValues;
}

void Morphology::command(const std::vector<double> &ctrl_com){
    if(ctrl_com.empty())
        return;
    double maxVelocity = settings::getParameter<settings::Double>(parameters,"#maxVelocity").value;
    energy_cost = 0;

    //Wheel Commands
    std::vector<double> wheel_com;
    wheel_com.insert(wheel_com.begin(),ctrl_com.begin(),ctrl_com.begin() + wheelHandles.size());
    //compute energy cost of this command
    for(const double& val: wheel_com)
        energy_cost+=fabs(val);
    sentCommandToWheels(wheelHandles,wheel_com,maxVelocity);

    //Joint Commands
    std::vector<double> joint_com;
    joint_com.insert(joint_com.begin(),ctrl_com.begin() + wheelHandles.size(), ctrl_com.end());
    std::vector<double> positions;
    getJointsPosition(jointHandles,positions);
    //compute energy cost of this command //TODO : different energy computation according to the type of joint controller
    for(size_t i = 0; i < positions.size();i++)
        energy_cost += fabs(positions[i]/M_PI*2.f - joint_com[i]);

    int ctrlType = settings::getParameter<settings::Integer>(parameters,"#jointControllerType").value;
    if(ctrlType == settings::jointCtrlType::DIRECT)
        sentCommandToJointsDirect(jointHandles,joint_com);
    else if(ctrlType == settings::jointCtrlType::PROPORTIONAL){
        float propGain = settings::getParameter<settings::Float>(parameters,"#proportionalGain").value;
        sentCommandToJointsProportional(jointHandles,joint_com,propGain);
    }else if(ctrlType == settings::jointCtrlType::OSCILLATORY){
        sentCommandToJointsOscillatory(jointHandles,joint_com);
    }
}
