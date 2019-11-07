#include "EPuckMorphology.h"
#include "v_repLib.h"

using namespace are;

void EPuckMorphology::create()
{
    std::string path_epuck_m = settings->vrep_folder + "models/epuck.ttm";
    simLoadModel(path_epuck_m.c_str());

    int epuckHandle = simGetObjectHandle("ePuck_respondableBody");

    mainHandle = epuckHandle;

    getObjectHandles();

    substrate.m_input_coords = {//Proximity center
                                {1,M_PI},{1,5.*M_PI/6.},
                                {1,2.*M_PI/3.},{1,M_PI/3.},
                                {1,M_PI/6.},{1,0},
                                {1,-M_PI/3.},{1,-2.*M_PI/3.},
                                //Front Camera average RGB
                                {1,M_PI/2.}, //R
                                {1,5.*M_PI/12.}, //G
                                {1,7.*M_PI/12.}}; //B
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

void EPuckMorphology::getObjectHandles()
{
    simAddObjectToSelection(sim_handle_tree, mainHandle);
    size_t select_size = static_cast<size_t>(simGetObjectSelectionSize());
    std::cout << "MORPHOLOGY INIT " << select_size << " objects selected in the epuck tree" << std::endl;
    int obj_handles[select_size];
    simGetObjectSelection(obj_handles);

    for (size_t i = 0; i < select_size; i++)
    {
        if(simGetObjectType(obj_handles[i]) == sim_object_joint_type)
            jointHandles.push_back(obj_handles[i]);
        else if(simGetObjectType(obj_handles[i]) == sim_object_visionsensor_type){
            std::cout << "MORPHOLOGY INIT camera added" << std::endl;
            cameraHandle = obj_handles[i];
        }
        else if(simGetObjectType(obj_handles[i]) == sim_object_proximitysensor_type)
            proxHandles.push_back(obj_handles[i]);
    }
    std::cout << "MORPHOLOGY INIT number of joint handles : " << jointHandles.size() << std::endl;
    std::cout << "MORPHOLOGY INIT number of proximity sensor handles : " << proxHandles.size() << std::endl;

}

std::vector<double> EPuckMorphology::update(){
    std::vector<double> sensorValues;

    std::function<double(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> double
    {return std::sqrt(x*x + y*y + z*z);};

    float pos[3], norm[3];
    int obj_h;
    for (size_t i = 0; i < proxHandles.size(); i++)
    {
        if(simHandleProximitySensor(proxHandles[i],pos,&obj_h,norm) >= 0)
             sensorValues.push_back(norm_L2(pos[0],pos[1],pos[2]));
        else std::cerr << "No detection on Proximity Sensor" << std::endl;
//        simReadProximitySensor(proxHandles[i],pos,&obj_h,norm);
    }

    float *auxVal = nullptr;
    int *auxCount = nullptr;
    if(simHandleVisionSensor(cameraHandle,&auxVal,&auxCount) >= 0)
    {
        if(auxCount[0] > 0 || auxCount[1] >=15)
        {
            sensorValues.push_back(auxVal[11]);
            sensorValues.push_back(auxVal[12]);
            sensorValues.push_back(auxVal[13]);
        }
        else std::cerr << "No Value read on the vision sensor" << std::endl;
        simReleaseBuffer((char *)auxVal);
        simReleaseBuffer((char *)auxCount);
    }
    else std::cerr << "No detection on CAMERA" << std::endl;

    return sensorValues;

}
