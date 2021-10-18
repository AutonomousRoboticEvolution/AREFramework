#include <sstream>
#include "simulatedER/coppelia_communication.hpp"
#include <cmath>

using namespace are;

// Coordination 2 real sensor value
// x = [-400mm, -300mm, -200mm, -100mm, 0mm, 100mm, 200mm, 300mm, 400mm]
const double coord2value_map[17][9] = 
    {{0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 2.8, 3.3, 6.0, 0, 6.0, 3.3, 2.8, 0},     // y = -200mm
    {0, 4.95, 7.75, 32.0, 0, 32.0, 7.75, 4.95, 0},   // y = -100mm
    {0, 10.1, 34.9, 233.35, 255, 233.35, 34.9, 10.1, 0},  // y = 0mm
    {0, 27.15, 159.45, 255, 255, 255, 159.45, 27.15, 0},
    {0, 87.05, 185.35, 255, 255, 255, 185.35, 87.05, 0},
    {0, 69.95, 120.1, 255, 255, 255, 120.1, 69.95, 0},
    {0, 50.25, 116.5, 255, 255, 255, 116.5, 50.25, 0},
    {0, 48.25, 134.6, 236.2, 255, 236.2, 134.6, 48.25, 0},
    {0, 60.75, 140.10, 248.3, 234.4, 248.3, 140.1, 60.75, 0},
    {0, 46.65, 116.3, 185.3, 287, 185.3, 116.3, 46.65, 0},
    {0, 54.1, 90.65, 124.4, 126.4, 124.4, 90.65, 54.1, 0},
    {0, 49.35, 79.9, 105.6, 100.7, 105.6, 79.9, 49.35, 0},
    {0, 48.8, 67.5, 87.1, 101.5, 87.1, 67.5, 48.8, 0},   // y = -1000mm
    {0, 0, 0, 0, 0, 0, 0, 0, 0}};

void sim::readProximitySensors(const std::vector<int> handles, std::vector<double> &sensorValues){

    std::function<double(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> double
    {return std::sqrt(x*x + y*y + z*z);};

    // map the simulation data to true sensor data
    std::function<double(float, float)> get_true_sensor_value = [](float x, float y) -> double{
        int locate_x = round(x * 1000 / 100);  // m -> mm -> int location in map
        int locate_y = round(y * 1000 / 100);
        float offset_x = x * 1000 / 100 - locate_x;  // float - int
        float offset_y = y * 1000 / 100 - locate_y;
        int axis_x = locate_x + 4;
        int axis_y = locate_y + 3;
        float sensor_value;

        if (axis_x < 1 || axis_x > 7 || axis_y < 1 || axis_y > 13){
            sensor_value = 0.0;
        }
        else{
            float base_value = coord2value_map[axis_y][axis_x];
            float offset_value;
            if (offset_x < -0.5 && offset_y > 0.5){
                offset_value = (coord2value_map[axis_y+1][axis_x-1] - coord2value_map[axis_y][axis_x]) * std::sqrt(offset_x*offset_x + offset_y*offset_y);
            }
            else if (offset_x >= -0.5 && offset_x <= 0.5 && offset_y > 0.5){
                offset_value = (coord2value_map[axis_y+1][axis_x] - coord2value_map[axis_y][axis_x]) * offset_y;
            }
            else if (offset_x > 0.5 && offset_y > 0.5){
                offset_value = (coord2value_map[axis_y+1][axis_x+1] - coord2value_map[axis_y][axis_x]) * std::sqrt(offset_x*offset_x + offset_y*offset_y);
            }
            else if (offset_x > 0.5 && offset_y <= 0.5 && offset_y >= -0.5){
                offset_value = (coord2value_map[axis_y][axis_x+1] - coord2value_map[axis_y][axis_x]) * offset_x;
            }
            else if (offset_x <= 0.5 && offset_x >= -0.5 && offset_y <= 0.5 && offset_y >= -0.5){
                offset_value = 0;
            }
            else if (offset_x < -0.5 && offset_y <= 0.5 && offset_y >= -0.5){
                offset_value = (coord2value_map[axis_y][axis_x-1] - coord2value_map[axis_y][axis_x]) * offset_x;
            }
            else if (offset_x > 0.5 && offset_y < -0.5){
                offset_value = (coord2value_map[axis_y-1][axis_x+1] - coord2value_map[axis_y][axis_x]) * std::sqrt(offset_x*offset_x + offset_y*offset_y);
            }
            else if (offset_x <= 0.5 && offset_x >= -0.5 && offset_y < -0.5){
                offset_value = (coord2value_map[axis_y-1][axis_x] - coord2value_map[axis_y][axis_x]) * offset_y;
            }
            else{
                offset_value = (coord2value_map[axis_y-1][axis_x-1] - coord2value_map[axis_y][axis_x]) * std::sqrt(offset_x*offset_x + offset_y*offset_y);
            }
            sensor_value = base_value + offset_value;
        }
        return sensor_value;
    };

    float pos[4], norm[3];
    int obj_h;
    int det;
    float true_sensor_value;
    for (size_t i = 0; i < handles.size(); i++)
    {
        det = simReadProximitySensor(handles[i],pos,&obj_h,norm);
        if(det > 0){
            true_sensor_value = get_true_sensor_value(pos[0], pos[1]);
            //sensorValues.push_back(norm_L2(pos[0],pos[1],pos[2]));
            sensorValues.push_back(true_sensor_value);
        }
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

void sim::robotScreenshot(int individual, int generation, std::string repository)
{
    // https://www.coppeliarobotics.com/helpFiles/en/regularApi/simCreateVisionSensor.htm
    const int resX = 1152, resY = 1152;
    const float nearClippingPlane = 0.02, farClippingPlane = 2.0, camAngle = M_PI_4, xSize = 0.01, ySize = 0.01, zSize = 0.01;
    int intParams[4] = {resX, resY, 0, 0};
    float floatParams[11] = {nearClippingPlane, farClippingPlane, camAngle, xSize, ySize, zSize, 0.0, 0.0, 0.0, 0.0, 0.0};
    int handleVisionSensor = simCreateVisionSensor(1, intParams, floatParams, NULL);
    int generalCamera;
    for(int i = 0; i<4; i++){
        if(i == 0)
            generalCamera = simGetObjectHandle("Camera0");
        else if(i == 1)
            generalCamera = simGetObjectHandle("Camera1");
        else if(i == 2)
            generalCamera = simGetObjectHandle("Camera2");
        else if(i == 3)
            generalCamera = simGetObjectHandle("Camera3");
        else
            abort();

        float identityMatrix[12];
        simBuildIdentityMatrix(identityMatrix);
        simSetObjectMatrix(handleVisionSensor, generalCamera, identityMatrix);
        simSetObjectParent(handleVisionSensor, generalCamera, true);
        simSetObjectInt32Parameter(handleVisionSensor,sim_visionintparam_resolution_x,resX);
        simSetObjectInt32Parameter(handleVisionSensor,sim_visionintparam_resolution_y,resY);
        simSetObjectFloatParameter(handleVisionSensor,sim_visionfloatparam_perspective_angle,camAngle*M_PI_2);

        simSetObjectInt32Parameter(handleVisionSensor,sim_visionintparam_pov_focal_blur,1);
        const float focalDistance = 2.0, aperture = 0.5;
        const int blurSamples = 10;
        simSetObjectFloatParameter(handleVisionSensor,sim_visionfloatparam_pov_blur_distance,focalDistance);
        simSetObjectFloatParameter(handleVisionSensor,sim_visionfloatparam_pov_aperture,aperture);
        simSetObjectInt32Parameter(handleVisionSensor,sim_visionintparam_pov_blur_sampled,blurSamples);

        simSetObjectInt32Parameter(handleVisionSensor,sim_visionintparam_render_mode,0);
        /// \todo Double-check this
        simInt savedVisibilityMask = 0;
        simGetObjectInt32Parameter(generalCamera,sim_objintparam_visibility_layer,&savedVisibilityMask);
        simSetObjectInt32Parameter(generalCamera,sim_objintparam_visibility_layer,0);

        int newAttr = sim_displayattribute_renderpass +  sim_displayattribute_forvisionsensor + sim_displayattribute_ignorerenderableflag;
        simSetObjectInt32Parameter(handleVisionSensor,sim_visionintparam_rendering_attributes,newAttr);
        float* auxValues=nullptr;
        int* auxValuesCount=nullptr;
        float averageColor[3]={0.0f,0.0f,0.0f};
        if (simHandleVisionSensor(handleVisionSensor,&auxValues,&auxValuesCount)>=0)
        {
            if ((auxValuesCount[0]>0)||(auxValuesCount[1]>=15))
            {
                averageColor[0]=auxValues[11];
                averageColor[1]=auxValues[12];
                averageColor[2]=auxValues[13];
            }
            simReleaseBuffer((char*)auxValues);
            simReleaseBuffer((char*)auxValuesCount);
        }

        simSetObjectInt32Parameter(generalCamera,sim_objintparam_visibility_layer,savedVisibilityMask);
        int resolutionX, resolutionY;
        const simUChar* image_buf_char = simGetVisionSensorCharImage(handleVisionSensor, &resolutionX, &resolutionY);
        std::stringstream filepath;
        filepath << repository << "/robot" << "_" << generation << "_" << individual << "_" << i << ".png";
        const std::string temp = filepath.str();

        simInt res[2];
        simGetVisionSensorResolution(handleVisionSensor,res);
        simSaveImage(image_buf_char,res,0,temp.c_str(),-1, nullptr);
        simReleaseBuffer((simChar*)image_buf_char);
    }
}