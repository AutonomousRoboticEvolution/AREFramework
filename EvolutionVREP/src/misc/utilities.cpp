#include "misc/utilities.h"

void misc::split_line(const std::string& line, const std::string& delim,
                std::list<std::string>& values){
    size_t pos = 0;
    std::string l = line;
    while ((pos = l.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = l.substr(0, pos);
        values.push_back(p);
        l = l.substr(pos + 1);
        pos = 0;
    }

    if (!l.empty()) {
//        split_line(l,delim,values);
        values.push_back(l);
    }
}

int sim::loadScene(int instance_type, const std::string &scene_path, int clientID)
{
    ///@todo coherent error code
    if(instance_type == are::settings::INSTANCE_REGULAR)
        return simLoadScene(scene_path.c_str());
    else if(instance_type == are::settings::INSTANCE_SERVER)
        return simxLoadScene(clientID,scene_path.c_str(),0,simx_opmode_blocking);
}

int sim::loadModel(int instance_type, const std::string &model_path,int clientID){
    if(instance_type == are::settings::INSTANCE_REGULAR)
        return simLoadModel(model_path.c_str());
    else if(instance_type == are::settings::INSTANCE_SERVER){
        int handle;
        simxLoadModel(clientID,model_path.c_str(),0,&handle,simx_opmode_blocking);
        return handle;
    }
}

int sim::setJointVelocity(int instance_type,
                           int jointHandle,
                           float value,
                          int clientID){
    if(instance_type == are::settings::INSTANCE_REGULAR)
        return simSetJointTargetVelocity(jointHandle,value);
    else if(instance_type == are::settings::INSTANCE_SERVER)
        return simxSetJointTargetVelocity(clientID, jointHandle, value, simx_opmode_oneshot);
}

int sim::pause(int instance_type, int clientID)
{
    if(instance_type == are::settings::INSTANCE_REGULAR)
        return simPauseSimulation();
    else if(instance_type == are::settings::INSTANCE_SERVER)
        return simxPauseSimulation(clientID,simx_opmode_oneshot);
}

void sim::setFloatingParameter(int instance_type, int param_name, float value, int clientID)
{
    if(instance_type == are::settings::INSTANCE_REGULAR)
        simSetFloatingParameter(param_name,value);
    else if(instance_type == are::settings::INSTANCE_SERVER)
        simxSetFloatingParameter(clientID,param_name,value,simx_opmode_oneshot);
}

void sim::addObjectToSelection(int instance_type, int mode, int handle, int clientID)
{
    if(instance_type == are::settings::INSTANCE_REGULAR)
        simAddObjectToSelection(mode,handle);
    else if(instance_type == are::settings::INSTANCE_SERVER)
        simxaddobject
}
