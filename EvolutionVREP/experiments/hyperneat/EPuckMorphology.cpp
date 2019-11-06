#include "EPuckMorphology.h"
#include "v_repLib.h"

using namespace are;

void EPuckMorphology::create()
{
    std::string path_epuck_m = settings->vrep_folder + "models/epuck.ttm";
    simLoadModel(path_epuck_m.c_str());

    int epuckHandle = simGetObjectHandle("ePuck_respondableBody");

    mainHandle = epuckHandle;

    substrate.m_input_coords = {{1,0},{1,M_PI/6.},
                                {1,M_PI/3.},{1,M_PI/2.},
                                {1,2.*M_PI/3.},{1,5.*M_PI/6.},
                                {1,M_PI},{1,-2.*M_PI/3.},
                                {1,-M_PI/3.}};
    substrate.m_output_coords = {{0.8,0},{0.8,M_PI}};
    substrate.m_hidden_coords = {{0,0},
                                 {0.5,0},{0.5,M_PI/6.},
                                 {0.5,M_PI/3.},{0.5,M_PI/2.},
                                 {0.5,2.*M_PI/3.},{0.5,5.*M_PI/6.},
                                 {0.5,M_PI},{0.5,-5.*M_PI/6.},
                                 {0.5,-2.*M_PI/3.},{0.5,-M_PI/2.},
                                 {0.5,-M_PI/3.},{0.5,-M_PI/6.}};

    std::cout << "Epuck Created" << std::endl;
}

void EPuckMorphology::createAtPosition(float x, float y, float z)
{
    create();
    setPosition(x,y,z);
}

void EPuckMorphology::setPosition(float x, float y, float z)
{
    float epuckPos[3];
    epuckPos[0] = x;
    epuckPos[1] = y;
    epuckPos[2] = z;

    simSetObjectPosition(mainHandle, -1, epuckPos);
}
