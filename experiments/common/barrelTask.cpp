#include "barrelTask.hpp"

#include <boost/algorithm/string.hpp>

using namespace are::sim;

BarrelTask::BarrelTask(const settings::ParametersMapPtr& params)
{
    parameters = params;

    final_position.resize(3);
    barrel_current_position.resize(3);
    name = "barrel_task";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#withBeacon",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));

    std::vector<double> targets = settings::getParameter<settings::Sequence<double>>(parameters,"#targets").value;
    for(int i = 0; i < targets.size();i+=3)
        barrel_initial_positions.push_back({targets[i],targets[i+1],targets[i+2]});

    target_position = settings::getParameter<settings::Sequence<double>>(parameters,"#targetPosition").value;
    barrel_handle = -1;
}

void BarrelTask::init(){

    VirtualEnvironment::init();

    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    std::string scenePath = settings::getParameter<settings::String>(parameters,"#scenePath").value;
    if(verbose){
        int i = 0;
        int handle = 0;
        std::cout << "Loaded scene : " << scenePath << std::endl;
        std::cout << "Objects in the scene : " << std::endl;
        while((handle = simGetObjects(i,sim_handle_all)) >= 0){
            std::cout << simGetObjectName(handle) << std::endl;
            i++;
        }
    }

    final_position = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    // Beacon
    bool withBeacon = settings::getParameter<settings::Boolean>(parameters,"#withBeacon").value;
    if(withBeacon){
        float bSize[3] = {0.1f,0.1f,0.1f};
        barrel_handle = simCreatePureShape(2,0,bSize,0.05f,nullptr); //create a sphere as beacon;

        simSetObjectName(barrel_handle,"IRBeacon_0");
        const float tPos[3] = {static_cast<float>(barrel_initial_positions[current_target][0]),
                         static_cast<float>(barrel_initial_positions[current_target][1]),
                         static_cast<float>(barrel_initial_positions[current_target][2])};

        if(simSetObjectPosition(barrel_handle,-1,tPos) < 0){
            std::cerr << "Set object position failed" << std::endl;
            exit(1);
        }
        simSetObjectSpecialProperty(barrel_handle,sim_objectspecialproperty_detectable_infrared);
//        simSetModelProperty(beacon_handle,sim_modelproperty_not_collidable | sim_modelproperty_not_dynamic);
        simSetObjectSpecialProperty(barrel_handle, sim_objectspecialproperty_collidable | sim_objectspecialproperty_measurable |
                                                   sim_objectspecialproperty_detectable_all | sim_objectspecialproperty_renderable); // Detectable, collidable, etc.
        simSetObjectInt32Parameter(barrel_handle, sim_shapeintparam_respondable, 1);
        simComputeMassAndInertia(barrel_handle, 0.162f);
        float color[3] = {0.1f,1.0f,0.1f};
        simSetShapeColor(barrel_handle, nullptr, sim_colorcomponent_ambient_diffuse, color);
    }
    trajectory.clear();

    // Create target
    float aruco_square_size[3] = {0.013f,0.013f,0.005f};
    std::vector<int> handles;
    float aruco_color[3] = {0.0f,0.0f,0.0f};
    float aruco_pos[3];
    float aruco_ori[3] = {0.,M_PI_2,0.};
    aruco_pos[0] = static_cast<float>(target_position.at(0));
    aruco_pos[1] = static_cast<float>(target_position.at(1));
    aruco_pos[2] = static_cast<float>(target_position.at(2));
    /// \todo: Eb - Hard coded values
    float pos_y[8] = {0.07,0.07,-0.07,-0.07,0.02,0.02,-0.02,-0.02};
    float pos_z[8] = {0.07,-0.07,-0.07,0.07,0.02,-0.02,-0.02,0.02};
    float temp_pos[3];
    for(int i = 0; i < 8; i++){
        handles.push_back(simCreatePureShape(0,0,aruco_square_size,0.05f,nullptr));
        temp_pos[0] = aruco_pos[0];
        temp_pos[1] = pos_y[i] + aruco_pos[1];
        if(i < 4) /// \todo: Eb - Hard coded values
            temp_pos[2] = 0.205;
        else
            temp_pos[2] = 0.09;
        temp_pos[2] = pos_z[i] + temp_pos[2];
        simSetObjectPosition(handles.at(i),-1,temp_pos);
        simSetShapeColor(handles.at(i), nullptr, sim_colorcomponent_ambient_diffuse, aruco_color);
        std::string string = "aruco_" + std::to_string(i);
        simSetObjectName(handles.at(i),string.c_str());
        simSetObjectOrientation(handles.at(i),handles.at(i),aruco_ori);
        simSetObjectInt32Parameter(handles.at(i), sim_shapeintparam_static, 1);
        simSetObjectSpecialProperty(handles.at(i), sim_objectspecialproperty_collidable | sim_objectspecialproperty_measurable |
                                                   sim_objectspecialproperty_detectable_all | sim_objectspecialproperty_renderable); // Detectable, collidable, etc.
    }


    std::vector<int> th;
    build_tiled_floor(th);
}

std::vector<double> BarrelTask::fitnessFunction(const Individual::Ptr &ind){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    double max_dist = sqrt(2*arena_size*arena_size);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    std::vector<double> d(1);
    d[0] = 1 - (distance(final_position,barrel_current_position)/max_dist)/2 - (distance(target_position,barrel_current_position)/max_dist)/2;

    for(double& f : d)
        if(std::isnan(f) || std::isinf(f) || f < 0)
            f = 0;
        else if(f > 1) f = 1;

    //Go to next target
    current_target+=1;
    if(current_target >= barrel_current_position.size())
        current_target=0;

    return d;
}


float BarrelTask::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = morph->getMainHandle();
    // Get robot position
    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);

    if(wp.is_nan())
        return 1;

    if(fabs(final_position[0] - wp.position[0]) > 1e-1 ||
            fabs(final_position[1] - wp.position[1]) > 1e-1 ||
            fabs(final_position[2] - wp.position[2]) > 1e-1)
        move_counter++;

    final_position[0] = static_cast<double>(wp.position[0]);
    final_position[1] = static_cast<double>(wp.position[1]);
    final_position[2] = static_cast<double>(wp.position[2]);

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);
    else if(simulationTime >= evalTime)
        trajectory.push_back(wp);

    // Get beacon position
    simGetObjectPosition(barrel_handle, -1, wp.position);
    simGetObjectOrientation(barrel_handle,-1,wp.orientation);

    if(wp.is_nan())
        return 1;

    barrel_current_position[0] = static_cast<double>(wp.position[0]);
    barrel_current_position[1] = static_cast<double>(wp.position[1]);
    barrel_current_position[2] = static_cast<double>(wp.position[2]);

    return 0;
}

void BarrelTask::build_tiled_floor(std::vector<int> &tiles_handles){
    bool flatFloor = settings::getParameter<settings::Boolean>(parameters,"#flatFloor").value;

    float tile_size[3] = {0.249f,0.249f,0.01f};
    float tile_increment = 0.25;
    float starting_pos[3] = {-0.875f,-0.875f,0.005f};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            tiles_handles.push_back(simCreatePureShape(0,8,tile_size,0.05f,nullptr));
            std::stringstream name;
            name << "tile_" << i << j;
            simSetObjectName(tiles_handles.back(),name.str().c_str());
            float height = -0.004;
            if(!flatFloor){
                height = randNum->randFloat(-0.005,-0.001);
            }
            float pos[3] = {starting_pos[0] + i*tile_increment,starting_pos[1] + j*tile_increment,height};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParameter(sim_bullet_body_friction,tiles_handles.back(),nullptr,1000);//randNum->randFloat(0,1000));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}


