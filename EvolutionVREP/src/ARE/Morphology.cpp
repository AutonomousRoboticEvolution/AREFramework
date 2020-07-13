#include "ARE/Morphology.h"

using namespace are;
namespace cop = coppelia;

std::vector<double> Morphology::update(){
    std::vector<double> sensorValues;

    cop::readProximitySensors(proxHandles,sensorValues);
    cop::readPassivIRSensors(IRHandles,sensorValues);

    return sensorValues;
}

void Morphology::command(const std::vector<double> &ctrl_com){
    double maxVelocity = settings::getParameter<settings::Double>(parameters,"#maxVelocity").value;
    energy_cost = 0;

    //Wheel Commands
    std::vector<double> wheel_com;
    wheel_com.insert(wheel_com.begin(),ctrl_com.begin(),ctrl_com.begin() + wheelHandles.size());
    //compute energy cost of this command
    for(const double& val: wheel_com)
        energy_cost+=fabs(val);
    cop::sentCommandToWheels(wheelHandles,wheel_com,maxVelocity);

    //Joint Commands
    std::vector<double> joint_com;
    joint_com.insert(joint_com.begin(),ctrl_com.begin() + wheelHandles.size(), ctrl_com.end());
    std::vector<double> positions;
    cop::getJointsPosition(jointHandles,positions);
    //compute energy cost of this command
    for(size_t i = 0; i < positions.size();i++)
        energy_cost += fabs(positions[i]/M_PI*2.f - joint_com[i]);
    cop::sentCommandToJoints(jointHandles,joint_com);
}
