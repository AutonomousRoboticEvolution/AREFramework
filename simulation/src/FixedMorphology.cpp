#include "simulatedER/FixedMorphology.hpp"

using namespace are::sim;


void FixedMorphology::create()
{
    retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles,camera_handle);
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose){
        std::cout << "Number of Proximity Sensors : " << proxHandles.size() << std::endl;
        std::cout << "Number of Passive IR Sensors : " << IRHandles.size() << std::endl;
        std::cout << "Number of Wheels : " << wheelHandles.size() << std::endl;
        std::cout << "Number of Joints : " << jointHandles.size() << std::endl;
    }

    //random orientation;
//    bool randOrient = settings::getParameter<settings::Boolean>(parameters,"#randomOrientation").value;
//    float robotOrient;
//    if(randOrient){
//        robotOrient = randomNum->randFloat(0,2*3.14);
//    }
//    else{
//        robotOrient = settings::getParameter<settings::Float>(parameters,"#robotOrientation").value;
//    }

//    float orientation[3];
//    simGetObjectOrientation(mainHandle,mainHandle,orientation);
//    orientation[0] = robotOrient;
    std::vector<double> orientation = settings::getParameter<settings::Sequence<double>>(parameters,"#robotOrientation").value;
    simSetObjectOrientation(mainHandle,-1,orientation.data());

    std::cout << "Robot Created" << std::endl;
}

void FixedMorphology::loadModel(){
    std::cout << "load model" << std::endl;

    std::string path = settings::getParameter<settings::String>(parameters,"#robotPath").value;
    int handle = simLoadModel(path.c_str());

    //sim::loadModel(instance_type,path_epuck_m.c_str(),client_id);
    if(handle == -1)
    {
        std::cerr << "unable to load robot model" << std::endl;
        char* lastError = simGetLastError();
        std::cerr << "simGetLastError : " << lastError << std::endl;
        simReleaseBuffer(lastError);
        exit(1);
    }

    mainHandle = handle;
}


void FixedMorphology::createAtPosition(float x, float y, float z)
{
    create();
    setPosition(x,y,z);
}

void FixedMorphology::setPosition(float x, float y, float z)
{
    double robotPos[3];
    robotPos[0] = x;
    robotPos[1] = y;
    robotPos[2] = z;
    if(simSetObjectPosition(mainHandle,-1,robotPos) < 0){
        std::cerr << "Set object position failed" << std::endl;
        exit(1);
    }
}

