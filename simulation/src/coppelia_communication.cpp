#include <sstream>
#include "simulatedER/coppelia_communication.hpp"

using namespace are;

// look up table for sensor reading from "20.40.025 (raw) data for organ calibrations"
// range from 0-1m in the z direction and 0-0.3m in the x direction
int lookup_table::coord2value_map[11][4] =
        {{255,233,35,10},
         {176,255,159,27},
         {255,255,185,87},
         {255,255,120,70},
         {255,255,117,50},
         {255,236,135,48},
         {234,248,140,61},
         {187,185,116,47},
         {126,124,91,54},
         {101,106,80,49},
         {102,87,68,49}};

//threshold for passive IR
int lookup_table::threshold = 100;


void sim::readProximitySensors(const std::vector<int> handles, std::vector<double> &sensorValues){
    // reads proximity sensor returning its distance to the pushback function

    // function for euclidean distance
    std::function<double(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> double
    {return std::sqrt(x*x + y*y + z*z);};
    
    // variables for proximity sensor function
    float pos[4], norm[3];
    int detected_object_handle;
    int det;
    // for all proximity sensors reads there vaule
    for (size_t i = 0; i < handles.size(); i++)
    {
        det = simReadProximitySensor(handles[i],pos,&detected_object_handle,norm);
        if(det > 0)
            sensorValues.push_back(norm_L2(pos[0],pos[1],pos[2]));
        else if(det <= 0) sensorValues.push_back(0);
    }
}

void sim::readPassivIRSensors(const std::vector<int> handles, std::vector<double> &sensorValues){
    // reads passive IR sensor taking into account object that block the view

    // function for euclidean distance
    std::function<float(float, float, float)> norm_L2 =
            [](float x, float y, float z) -> float { return std::sqrt(x * x + y * y + z * z); };

    int occlusion_detector;

    // variables for proximity sensor function
    float norm[3];
    float position_occlusion[4];
    float position_passive[4];
    int detected_object_handle;
    int det, occl;
    std::string name;
    std::vector<std::string> splitted_name;
    bool occlusion = false;

    for (int handle: handles) {

        occlusion_detector = simGetObjectChild(handle, 0);
        det = simReadProximitySensor(handle, position_passive, &detected_object_handle, norm);
        // calculates distance to object
        float dist = norm_L2(position_passive[0], position_passive[1], position_passive[2]);
        // Did the sensor detected something?
        if (det > 0) {
            name = simGetObjectName(detected_object_handle);
            float ref_euler[3];
            if (position_passive[0] == 0) position_passive[1] += 1e-3; // small inaccuracy in case of x = 0;
            float euler[3] = {static_cast<float>(std::atan2(position_passive[2], position_passive[1]) - M_PI / 2.f),
                              static_cast<float>(std::asin(position_passive[0] / dist)),
                              0};
            // Rotates the
            simSetObjectOrientation(occlusion_detector, handle, euler);
            occl = simReadProximitySensor(occlusion_detector, position_occlusion, &detected_object_handle, norm);
            // Checks occlusion exists and is closer than the beacon
            if (occl > 0) {
                occlusion = norm_L2(position_occlusion[0], position_occlusion[1], position_occlusion[2]) < dist;
            } else occlusion = false;
            boost::split(splitted_name, name, boost::is_any_of("_"));

            // Only detects IR beacon
            if (splitted_name[0] == "IRBeacon" && !occlusion) {

                //corrects to center of beacon as 5cm further away
                float x_pos = sqrt(position_passive[1] * position_passive[1] + position_passive[0] * position_passive[0]);
                float angle = atan2(x_pos, position_passive[2]);
                float r = std::sqrt(position_passive[2] * position_passive[2] + x_pos * x_pos);
                r = r + 0.05;//adds radius of beacon
                x_pos = r * std::sin(angle);
                float z_pos = r * std::cos(angle);
                // Calculate z and x coordinate in demimeters
                int int_z = round(z_pos * 10.);
                int int_x = round(x_pos * 10.);
                int IR_reading = 0;
                // Error check for bad values
                if (int_z < 11 && int_x < 4 && int_z >= 0)
                    IR_reading = lookup_table::coord2value_map[int_z][int_x];
                // For high reading return true value
                if (IR_reading >= lookup_table::threshold) {
                    sensorValues.push_back(1);
                } else sensorValues.push_back(0);

            } else // Beacon not found
                sensorValues.push_back(0);

        } else if (det <= 0) // Nothing found
            sensorValues.push_back(0);

    }
}

void sim::readCamera(const int camera_handle, std::vector<double> &sensorValues){
    std::vector<int> big_corner_handles;
    std::vector<int> small_corner_handles;
    int nbrObj = 0;
    int* handles = nullptr;
    handles = simGetObjectsInTree(sim_handle_scene,sim_handle_all,1,&nbrObj);
    std::string name;
    std::vector<std::string> splitted_name;
    for(int i = 0; i < nbrObj ; i++){
        name = simGetObjectName(handles[i]);
        boost::split(splitted_name,name,boost::is_any_of("_"));
        if(splitted_name[0] == "aruco"){
            if(std::stoi(splitted_name[1]) < 4)
                big_corner_handles.push_back(handles[i]);
            else
                small_corner_handles.push_back(handles[i]);
        }
    }
    simReleaseBuffer((simChar*)handles);
    if(big_corner_handles.size() < 1 || big_corner_handles.size() < 1){
        sensorValues.push_back(0);
        return;
    }

    float pos[3];
    int big_result= -1;
    int small_result= -1;
    for(int i = 0; i < big_corner_handles.size(); i++){
        big_result = simCheckProximitySensor(camera_handle,big_corner_handles[i],pos);
        small_result = simCheckProximitySensor(camera_handle,small_corner_handles[i],pos);
        if(big_result == 0 && small_result == 0){
            sensorValues.push_back(0);
            return;
        }
    }
    sensorValues.push_back(1);
}

void sim::retrieveOrganHandles(int mainHandle, std::vector<int> &proxHandles, std::vector<int> &IRHandles,
                          std::vector<int> &wheelHandles, std::vector<int> &jointHandles, int &camera_handle){

    int nbrObj = 0;
    int* handles = nullptr;
    handles = simGetObjectsInTree(mainHandle,sim_object_proximitysensor_type,1,&nbrObj);

    std::string name;
    std::vector<std::string> splitted_name;
    for(int i = 0; i < nbrObj ; i++){
        name = simGetObjectName(handles[i]);
        std::cout << name << std::endl;
        boost::split(splitted_name,name,boost::is_any_of("_"));
        if(splitted_name[0] == "Proximity")
            proxHandles.push_back(handles[i]);
        else if(splitted_name[0] == "passivIR")
            IRHandles.push_back(handles[i]);
        else if(splitted_name[0] == "camera")
            camera_handle = handles[i];
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
        float actual_pos;
//        simGetJointPosition(handles[i], &actual_pos);
        simGetJointTargetPosition(handles[i], &actual_pos);
        double next_pos = misc::get_next_joint_position(commands[i], simulationTime, actual_pos);
        simSetJointTargetPosition(handles[i],static_cast<float>(next_pos));
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

void sim::robotScreenshot(int ind_id, std::string repository)
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
        filepath << repository << "/robot" << "_" << ind_id << "_" << i << ".png";
        const std::string temp = filepath.str();

        simInt res[2];
        simGetVisionSensorResolution(handleVisionSensor,res);
        simSaveImage(image_buf_char,res,0,temp.c_str(),-1, nullptr);
        simReleaseBuffer((simChar*)image_buf_char);
    }
}
