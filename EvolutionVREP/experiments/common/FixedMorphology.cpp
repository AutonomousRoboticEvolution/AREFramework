#include "FixedMorphology.hpp"

using namespace are;

NEAT::Substrate subtrates::are_puck([]() -> NEAT::Substrate {
                                        NEAT::Substrate sub;
                                        sub.m_input_coords = {{1,M_PI/2.}, {1,M_PI/4.},
                                                              {1,3.*M_PI/4.},{1,3.*M_PI/2.}};

                                        sub.m_output_coords = {{1,0},{1,M_PI}};
                                        sub.m_hidden_coords = {{0,0},
                                                               {0.5,0},{0.5,M_PI/6.},
                                                               {0.5,M_PI/3.},{0.5,M_PI/2.},
                                                               {0.5,2.*M_PI/3.},{0.5,5.*M_PI/6.},
                                                               {0.5,M_PI},{0.5,-5.*M_PI/6.},
                                                               {0.5,-2.*M_PI/3.},{0.5,-M_PI/2.},
                                                               {0.5,-M_PI/3.},{0.5,-M_PI/6.}};

                                        sub.m_allow_hidden_hidden_links = true;
                                        sub.m_query_weights_only = true;
                                        return sub;
                                    }());


NEAT::Substrate subtrates::epuck([]() -> NEAT::Substrate {
                                     NEAT::Substrate sub;
                                     sub.m_input_coords = {//Proximity center
                                                           {1,M_PI},{1,5.*M_PI/6.},
                                                           {1,2.*M_PI/3.},{1,M_PI/3.},
                                                           {1,M_PI/6.},{1,0},
                                                           {1,-M_PI/3.},{1,-2.*M_PI/3.}};
                                     //                                //Front Camera average RGB
                                     //                                {1,M_PI/2.}, //R
                                     //                                {1,5.*M_PI/12.}, //G
                                     //                                {1,7.*M_PI/12.}}; //B
                                     sub.m_output_coords = {{0.8,0},{0.8,M_PI}};
                                     sub.m_hidden_coords = {{0,0},
                                                            {0.5,0},{0.5,M_PI/6.},
                                                            {0.5,M_PI/3.},{0.5,M_PI/2.},
                                                            {0.5,2.*M_PI/3.},{0.5,5.*M_PI/6.},
                                                            {0.5,M_PI},{0.5,-5.*M_PI/6.},
                                                            {0.5,-2.*M_PI/3.},{0.5,-M_PI/2.},
                                                            {0.5,-M_PI/3.},{0.5,-M_PI/6.}};

                                     sub.m_allow_hidden_hidden_links = true;
                                     sub.m_query_weights_only = true;
                                     return sub;
                                 }());

void FixedMorphology::create()
{
    getObjectHandles();
    std::cout << "Robot Created" << std::endl;
}

void FixedMorphology::loadModel(){
    std::cout << "load model" << std::endl;

    std::string path_arepuck_m = settings::getParameter<settings::String>(parameters,"#robotPath").value;
    int arepuckHandle = simLoadModel(path_arepuck_m.c_str());

    //sim::loadModel(instance_type,path_epuck_m.c_str(),client_id);
    if(arepuckHandle == -1)
    {
        std::cerr << "unable to load arepuck model" << std::endl;
        simChar* lastError = simGetLastError();
        std::cerr << "simGetLastError : " << lastError << std::endl;
        simReleaseBuffer(lastError);
        exit(1);
    }

    mainHandle = arepuckHandle;
}


void FixedMorphology::createAtPosition(float x, float y, float z)
{
    create();
    setPosition(x,y,z);
}

void FixedMorphology::setPosition(float x, float y, float z)
{
    float epuckPos[3];
    epuckPos[0] = x;
    epuckPos[1] = y;
    epuckPos[2] = z;
    simSetObjectPosition(mainHandle,-1,epuckPos);

    //random orientation;
    float orientation[3];
    simGetObjectOrientation(mainHandle,mainHandle,orientation);
    orientation[0] = randomNum->randFloat(0,2*3.14);
    simSetObjectOrientation(mainHandle,mainHandle,orientation);
}

void FixedMorphology::getObjectHandles()
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    int nbrObj = 0;
    int* handles = nullptr;
    handles = simGetObjectsInTree(mainHandle,sim_object_proximitysensor_type,1,&nbrObj);

    if(verbose)
        std::cout << "MORPHOLOGY INIT number of proximity sensor handles : " << nbrObj << std::endl;
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
    if(verbose)
        std::cout << "MORPHOLOGY INIT number of joint handles : " << nbrObj << std::endl;
    for(int i = 0; i < nbrObj ; i++)
        jointHandles.push_back(handles[i]);

    simReleaseBuffer((simChar*)handles);
}

std::vector<double> FixedMorphology::update(){


    std::vector<double> sensorValues;

    readProximitySensors(sensorValues);
    readPassivIRSensors(sensorValues);

    return sensorValues;
}

void FixedMorphology::readProximitySensors(std::vector<double> &sensorValues){

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

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
        else
        {
            sensorValues.push_back(0);
            if(verbose)
                std::cerr << "No detection on Proximity Sensor" << std::endl;
        }

    }
}

void FixedMorphology::readPassivIRSensors(std::vector<double> &sensorValues){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;


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
    for (int handle : IRHandles) {
        occlusion_detector = simGetObjectChild(handle,0);

        det = simReadProximitySensor(handle,pos,&obj_h,norm);
        float dist = norm_L2(pos[0],pos[1],pos[2]);
        if(det > 0){

            name = simGetObjectName(obj_h);
            float ref_euler[3];
            if(pos[0] == 0) pos[1]+=1e-3; // small inaccuracy in case of x = 0;
            float euler[3] = {std::atan2(pos[2],pos[1]) - M_PI/2.f,
                              std::asin(pos[0]/dist),
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

        if(det <= 0){
            sensorValues.push_back(0);
            if(verbose)
                std::cerr << "No detetion on Passiv IR sensor" << std::endl;
        }

    }
}
