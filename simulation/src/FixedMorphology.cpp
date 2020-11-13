#include "simulatedER/FixedMorphology.hpp"

using namespace are::sim;

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
    retrieveOrganHandles(mainHandle,proxHandles,IRHandles,wheelHandles,jointHandles);
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose){
        std::cout << "Number of Proximity Sensors : " << proxHandles.size() << std::endl;
        std::cout << "Number of Passive IR Sensors : " << IRHandles.size() << std::endl;
        std::cout << "Number of Wheels : " << wheelHandles.size() << std::endl;
        std::cout << "Number of Joints : " << jointHandles.size() << std::endl;
    }

    //random orientation;
    bool randOrient = settings::getParameter<settings::Boolean>(parameters,"#randomOrientation").value;
    float robotOrient;
    if(randOrient){
        robotOrient = randomNum->randFloat(0,2*3.14);
    }
    else{
        robotOrient = settings::getParameter<settings::Float>(parameters,"#robotOrientation").value;
    }

    float orientation[3];
    simGetObjectOrientation(mainHandle,mainHandle,orientation);
    orientation[0] = robotOrient;
    simSetObjectOrientation(mainHandle,mainHandle,orientation);

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
    float robotPos[3];
    robotPos[0] = x;
    robotPos[1] = y;
    robotPos[2] = z;
    simSetObjectPosition(mainHandle,-1,robotPos);
}

