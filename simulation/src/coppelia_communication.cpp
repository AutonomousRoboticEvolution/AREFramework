#include "simulatedER/coppelia_communication.hpp"

using namespace are;

void sim::readProximitySensors(const std::vector<int> handles, std::vector<double> &sensorValues){

    std::function<double(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> double
    {return std::sqrt(x*x + y*y + z*z);};

    float pos[4], norm[3];
    int obj_h;
    int det;
    for (size_t i = 0; i < handles.size(); i++)
    {
        det = simReadProximitySensor(handles[i],pos,&obj_h,norm);
        if(det > 0)
            sensorValues.push_back(norm_L2(pos[0],pos[1],pos[2]));
        else if(det <= 0) sensorValues.push_back(0);
    }
}

void sim::readPassivIRSensors(const std::vector<int> handles, std::vector<double> &sensorValues){
    std::function<float(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> float
    {return std::sqrt(x*x + y*y + z*z);};

    int occlusion_detector;

    float pos[4], norm[3];
    int obj_h;
    int det,occl;
    std::string name;
    std::vector<std::string> splitted_name;
    bool occlusion = false;
    for (int handle : handles) {
        occlusion_detector = simGetObjectChild(handle,0);

        det = simReadProximitySensor(handle,pos,&obj_h,norm);
        float dist = norm_L2(pos[0],pos[1],pos[2]);
        if(det > 0){

            name = simGetObjectName(obj_h);
            float ref_euler[3];
            if(pos[0] == 0) pos[1]+=1e-3; // small inaccuracy in case of x = 0;
            float euler[3] = {static_cast<float>(std::atan2(pos[2],pos[1]) - M_PI/2.f),
                              static_cast<float>(std::asin(pos[0]/dist)),
                              0};
            simSetObjectOrientation(occlusion_detector,handle,euler);
            occl = simReadProximitySensor(occlusion_detector,pos,&obj_h,norm);
            if(occl > 0){
                occlusion = norm_L2(pos[0],pos[1],pos[2]) < dist;
            }else occlusion = false;

            boost::split(splitted_name,name,boost::is_any_of("_"));
            if(splitted_name[0] == "IRBeacon" && !occlusion)
                sensorValues.push_back(1);
            else sensorValues.push_back(0);
        }
        else if(det <= 0)
            sensorValues.push_back(0);
    }
}

void sim::retrieveOrganHandles(int mainHandle, std::vector<int> &proxHandles, std::vector<int> &IRHandles,
                          std::vector<int> &wheelHandles, std::vector<int> &jointHandles){

    int nbrObj = 0;
    int* handles = nullptr;
    handles = simGetObjectsInTree(mainHandle,sim_object_proximitysensor_type,1,&nbrObj);

    std::string name;
    std::vector<std::string> splitted_name;
    for(int i = 0; i < nbrObj ; i++){
        name = simGetObjectName(handles[i]);
        boost::split(splitted_name,name,boost::is_any_of("_"));
        if(splitted_name[0] == "Proximity")
            proxHandles.push_back(handles[i]);
        else if(splitted_name[0] == "passivIR")
            IRHandles.push_back(handles[i]);
    }

    handles = simGetObjectsInTree(mainHandle,sim_object_joint_type,1,&nbrObj);

    for(int i = 0; i < nbrObj ; i++){
        name = simGetObjectName(handles[i]);
        boost::split(splitted_name,name,boost::is_any_of("_"));
        if(splitted_name[0] == "Motor")
            wheelHandles.push_back(handles[i]);
        else jointHandles.push_back(handles[i]);
    }

    simReleaseBuffer((simChar*)handles);
}

void sim::sentCommandToJointsDirect(const std::vector<int>& handles,const std::vector<double>& commands){
    for (size_t i = 0; i < handles.size(); i++)
        simSetJointTargetPosition(handles[i],static_cast<float>(commands[i]*M_PI/2.));
}

void sim::sentCommandToJointsProportional(const std::vector<int>& handles, const std::vector<double>& commands, double P){

    for (size_t i = 0; i < handles.size(); i++){
        float currentPos;
        simGetJointPosition(handles[i],&currentPos);
        float e = P*(commands[i]*M_PI/2. - currentPos);
        simSetJointTargetPosition(handles[i],static_cast<float>((e+currentPos)));
    }
}

void sim::sentCommandToJointsOscillatory(const std::vector<int> &handles, const std::vector<double> &commands)
{
    double simulationTime = simGetSimulationTime();
    for (size_t i = 0; i < handles.size(); i++){
        double angle = sin(simulationTime*commands[i]);
        simSetJointTargetPosition(handles[i],static_cast<float>(angle*M_PI/2.));
    }
}

void sim::getJointsPosition(const std::vector<int>& handles,std::vector<double>& positions){
    float pos;
    for(const int& handle : handles){
        simGetJointPosition(handle,&pos);
        positions.push_back(static_cast<double>(pos));
    }
}

void sim::sentCommandToWheels(const std::vector<int>& handles, const std::vector<double>& commands, double max_velocity){
    for (size_t i = 0; i < handles.size(); i++)
        simSetJointTargetVelocity(handles[i],static_cast<float>(commands[i]*max_velocity));
}

void sim::printSimulatorState(int simState){
    std::cout << "Simulator State : ";
    if(simState == sim_simulation_stopped)
        std::cout << "STOPPED" << std::endl;
    else if(simState == sim_simulation_paused)
        std::cout << "PAUSED" << std::endl;
    else if(simState == sim_simulation_advancing)
        std::cout << "ADVANCING" << std::endl;
    else if(simState == sim_simulation_advancing_running)
        std::cout << "ADVANCING RUNNING" << std::endl;
    else if(simState == sim_simulation_advancing_abouttostop)
        std::cout << "ADVANCING ABOUT TO STOP" << std::endl;
    else if(simState == sim_simulation_advancing_firstafterstop)
        std::cout << "ADVANCING FIRST AFTER STOP" << std::endl;
    else if(simState == sim_simulation_advancing_lastbeforestop)
        std::cout << "ADVANCING LAST BEFORE STOP" << std::endl;
    else if(simState == sim_simulation_advancing_firstafterpause)
        std::cout << "ADVANCING FIRST AFTER PAUSE" << std::endl;
    else if(simState == sim_simulation_advancing_lastbeforepause)
        std::cout << "ADVANCING LAST BEFORE PAUSE" << std::endl;
    else std::cout << "UNKNOWN" << std::endl;
}
