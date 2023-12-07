#include "simulatedER/VirtualEnvironment.hpp"

using namespace are::sim;

void VirtualEnvironment::sceneLoader() {
    std::string scene_path = settings::getParameter<settings::String>(parameters,"#scenePath").value;
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(simLoadScene(scene_path.c_str()) < 0){
        std::cerr << "unable to load the scene : " << scene_path << std::endl;
        exit(1);
    }
    if(verbose){
        int i = 0;
        int handle = 0;
        std::cout << "Loaded scene : " << scene_path << std::endl;
        std::cout << "Objects in the scene : " << std::endl;
        while((handle = simGetObjects(i,sim_handle_all)) >= 0){
            std::cout << simGetObjectAlias(handle,0) << std::endl;
            i++;
        }
    }
}

void VirtualEnvironment::init() {
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value){
        std::cout << "Initialize Environment" << std::endl;
    }
    sceneLoader();
    maxTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    float time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    bool real_time = settings::getParameter<settings::Boolean>(parameters,"#realTimeSim").value;
    // Sets time step
    if(!real_time)
        simSetFloatParam(sim_floatparam_simulation_time_step,time_step);
    simSetBoolParam(sim_boolparam_realtime_simulation,real_time);
}

void VirtualEnvironment::build_tiled_floor(std::vector<int> &tiles_handles){
    bool flatFloor = settings::getParameter<settings::Boolean>(parameters,"#flatFloor").value;

    double tile_size[3] = {0.249f,0.249f,0.01f};
    double tile_increment = 0.25;
    double starting_pos[3] = {-0.875f,-0.875f,0.005f};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            tiles_handles.push_back(simCreatePrimitiveShape(sim_primitiveshape_cuboid,tile_size,0));
            simSetShapeMass(tiles_handles.back(),0.05f);
            std::stringstream name;
            name << "tile_" << i << j;
            simSetObjectAlias(tiles_handles.back(),name.str().c_str(),0);
            float height = -0.004;
            if(!flatFloor){
                height = (i+j)%2 == 0 ? -0.004 : 0.006;
//                height = randNum->randFloat(-0.05,-0.01);
            }
            double pos[3] = {starting_pos[0] + i*tile_increment,starting_pos[1] + j*tile_increment,height};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParam(sim_bullet_body_friction,tiles_handles.back(),nullptr,1000);//randNum->randFloat(0,1000));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}


std::vector<double> VirtualEnvironment::get_object_position(int handle){
    double pos[3];
    simGetObjectPosition(handle,-1,pos);
    return std::vector({pos[0],pos[1],pos[3]});
}
