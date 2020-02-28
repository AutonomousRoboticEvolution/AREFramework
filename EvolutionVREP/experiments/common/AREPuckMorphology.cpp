#include "AREPuckMorphology.h"
#include "v_repLib.h"


using namespace are;

void AREPuckMorphology::create()
{
    getObjectHandles();
    initSubstrate();
    std::cout << "AREpuck Created" << std::endl;
}

void AREPuckMorphology::loadModel(){
    std::cout << "load model" << std::endl;

    std::string path_epuck_m = settings::getParameter<settings::String>(parameters,"#robotPath").value;
    int epuckHandle = simLoadModel(path_epuck_m.c_str());

            //sim::loadModel(instance_type,path_epuck_m.c_str(),client_id);
    if(epuckHandle == -1)
    {
        std::cerr << "unable to load epuck model" << std::endl;
        simChar* lastError = simGetLastError();
        std::cerr << "simGetLastError : " << lastError << std::endl;
        simReleaseBuffer(lastError);
        exit(1);
    }

    mainHandle = epuckHandle;
}

void AREPuckMorphology::initSubstrate(){
    substrate.m_input_coords = {{1,M_PI/2.}, {1,M_PI/4.},
                                {1,3.*M_PI/4.},{1,3.*M_PI/2.}};

    substrate.m_output_coords = {{1,0},{1,M_PI}};
    substrate.m_hidden_coords = {{0,0},
                                 {0.5,0},{0.5,M_PI/6.},
                                 {0.5,M_PI/3.},{0.5,M_PI/2.},
                                 {0.5,2.*M_PI/3.},{0.5,5.*M_PI/6.},
                                 {0.5,M_PI},{0.5,-5.*M_PI/6.},
                                 {0.5,-2.*M_PI/3.},{0.5,-M_PI/2.},
                                 {0.5,-M_PI/3.},{0.5,-M_PI/6.}};

    substrate.m_allow_hidden_hidden_links = true;
    substrate.m_query_weights_only = true;
}

void AREPuckMorphology::createAtPosition(float x, float y, float z)
{
    create();
    setPosition(x,y,z);
}

void AREPuckMorphology::setPosition(float x, float y, float z)
{
    float epuckPos[3];
    epuckPos[0] = x;
    epuckPos[1] = y;
    epuckPos[2] = z;

    simSetObjectPosition(mainHandle,-1,epuckPos);
}

void AREPuckMorphology::getObjectHandles()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    int nbrObj = 0;
    int* handles = nullptr;
    handles = simGetObjectsInTree(mainHandle,sim_object_proximitysensor_type,1,&nbrObj);

    if(verbose)
        std::cout << "MORPHOLOGY INIT number of proximity sensor handles : " << nbrObj << std::endl;
    for(int i = 0; i < nbrObj ; i++)
        proxHandles.push_back(handles[i]);

    handles = simGetObjectsInTree(mainHandle,sim_object_joint_type,1,&nbrObj);
    if(verbose)
        std::cout << "MORPHOLOGY INIT number of joint handles : " << nbrObj << std::endl;
    for(int i = 0; i < nbrObj ; i++)
        jointHandles.push_back(handles[i]);

    simReleaseBuffer((simChar*)handles);
}

std::vector<double> AREPuckMorphology::update(){

    std::vector<double> sensorValues;

    std::function<double(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> double
    {return std::sqrt(x*x + y*y + z*z);};

    float pos[4], norm[3];
    int obj_h;
    int det;
    for (size_t i = 0; i < proxHandles.size(); i++)
    {
        det = simReadProximitySensor(proxHandles[i],pos,&obj_h,norm);
        if(det > 0)
            sensorValues.push_back(norm_L2(pos[0],pos[1],pos[2]));
        else if(det == 0) sensorValues.push_back(0);
        else std::cerr << "No detection on Proximity Sensor" << std::endl;
    }
    return sensorValues;
}
