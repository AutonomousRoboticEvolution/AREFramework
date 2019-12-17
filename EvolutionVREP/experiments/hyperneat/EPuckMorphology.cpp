#include "EPuckMorphology.h"
#include "v_repLib.h"


using namespace are;

void EPuckMorphology::create()
{
    mainHandle = simGetObjectHandle("ePuck_respondableBody");
    getObjectHandles();
    initSubstrate();
    std::cout << "Epuck Created" << std::endl;
}

void EPuckMorphology::loadModel(){
    std::cout << "load model" << std::endl;
    int instance_type = settings::INSTANCE_REGULAR;//getParameter<settings::Integer>(parameters,"#instanceType").value;


    std::string path_epuck_m = settings::getParameter<settings::String>(parameters,"#epuckPath").value;
    int epuckHandle = sim::loadModel(instance_type,path_epuck_m.c_str(),client_id);
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

void EPuckMorphology::initSubstrate(){
    substrate.m_input_coords = {//Proximity center
                                {1,M_PI},{1,5.*M_PI/6.},
                                {1,2.*M_PI/3.},{1,M_PI/3.},
                                {1,M_PI/6.},{1,0},
                                {1,-M_PI/3.},{1,-2.*M_PI/3.}};
//                                //Front Camera average RGB
//                                {1,M_PI/2.}, //R
//                                {1,5.*M_PI/12.}, //G
//                                {1,7.*M_PI/12.}}; //B
    substrate.m_output_coords = {{0.8,0},{0.8,M_PI}};
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

    sim::setObjectPosition(
                settings::INSTANCE_REGULAR,
                mainHandle, -1, epuckPos,client_id);
}

void EPuckMorphology::getObjectHandles()
{

    int instance_type = settings::INSTANCE_REGULAR; //getParameter<settings::Integer>(parameters,"#instanceType").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    if(instance_type == settings::INSTANCE_REGULAR)
    {
        simAddObjectToSelection(sim_handle_tree, mainHandle);
        size_t select_size = static_cast<size_t>(simGetObjectSelectionSize());
        if(verbose)
            std::cout << "MORPHOLOGY INIT " << select_size << " objects selected in the epuck tree" << std::endl;
        int obj_handles[select_size];
        simGetObjectSelection(obj_handles);

        for (size_t i = 0; i < select_size; i++)
        {
            if(simGetObjectType(obj_handles[i]) == sim_object_joint_type)
                jointHandles.push_back(obj_handles[i]);
            else if(simGetObjectType(obj_handles[i]) == sim_object_visionsensor_type){
                if(verbose)
                    std::cout << "MORPHOLOGY INIT camera added" << std::endl;
                cameraHandle = obj_handles[i];
            }
            else if(simGetObjectType(obj_handles[i]) == sim_object_proximitysensor_type)
                proxHandles.push_back(obj_handles[i]);
        }
        if(verbose){
            std::cout << "MORPHOLOGY INIT number of joint handles : " << jointHandles.size() << std::endl;
            std::cout << "MORPHOLOGY INIT number of proximity sensor handles : " << proxHandles.size() << std::endl;
        }
    }
    else if(instance_type == settings::INSTANCE_SERVER)
    {
        int nbrObj = 0;
        int* handles = nullptr;
        simxGetObjects(client_id,sim_object_proximitysensor_type,&nbrObj,&handles,simx_opmode_blocking);

        if(verbose)
            std::cout << "MORPHOLOGY INIT number of proximity sensor handles : " << nbrObj << std::endl;
        for(int i = 0; i < nbrObj ; i++)
            proxHandles.push_back(handles[i]);

        simxGetObjects(client_id,sim_object_joint_type,&nbrObj,&handles,simx_opmode_blocking);
        if(verbose)
            std::cout << "MORPHOLOGY INIT number of joint handles : " << nbrObj << std::endl;
        for(int i = 0; i < nbrObj ; i++)
            jointHandles.push_back(handles[i]);
    }

}

std::vector<double> EPuckMorphology::update(){

    int instance_type = settings::INSTANCE_REGULAR;//settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    std::vector<double> sensorValues;

    std::function<double(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> double
    {return std::sqrt(x*x + y*y + z*z);};

    float pos[4], norm[3];
    int obj_h;
    int det;
//    sim::pauseCommunication(instance_type,1,client_id);
    for (size_t i = 0; i < proxHandles.size(); i++)
    {
//        simHandleProximitySensor(proxHandles[i],pos,&obj_h,norm);
        det = sim::readProximitySensor(instance_type,proxHandles[i],pos,&obj_h,norm,client_id);
        if(det > 0)
            sensorValues.push_back(norm_L2(pos[0],pos[1],pos[2]));
        else if(det == 0) sensorValues.push_back(-1);
        else std::cerr << "No detection on Proximity Sensor" << std::endl; //<< simGetLastError() << std::endl;
    }
//    sim::pauseCommunication(instance_type,0,client_id);

//    float *auxVal = nullptr;
//    int *auxCount = nullptr;
//    simHandleVisionSensor(cameraHandle,&auxVal,&auxCount);
//    if(simReadVisionSensor(cameraHandle,&auxVal,&auxCount) >= 0)
//    {
//        for(int i = 0; i < auxCount[0]; i++)
//            std::cout << auxVal[i] << std::endl;
//        if(auxCount[0] > 0 && auxCount[1] >=15)
//        {
//            sensorValues.push_back(auxVal[11]);
//            sensorValues.push_back(auxVal[12]);
//            sensorValues.push_back(auxVal[13]);
//        }
//        else std::cerr << "No Value read on the vision sensor" << std::endl;
//        simReleaseBuffer((char *)auxVal);
//        simReleaseBuffer((char *)auxCount);
//    }
//    else std::cerr << "No detection on CAMERA " << std::endl ;//<< simGetLastError() << std::endl;

    return sensorValues;

}
