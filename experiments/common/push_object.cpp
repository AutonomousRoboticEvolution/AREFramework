#include "push_object.hpp"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include <boost/algorithm/string.hpp>
#include <simulatedER/Morphology.h>

using namespace are;
using namespace are::sim;

PushObject::PushObject(const settings::ParametersMapPtr& params)
{
    parameters = params;
    final_position.resize(3);
    object_initial_position.resize(3);
    object_current_position.resize(3);
    name = "push_object";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#withTiles",new settings::Boolean(false));

}

void PushObject::init(){

    VirtualEnvironment::init();

    object_initial_position = settings::getParameter<settings::Sequence<double>>(parameters,"#targetPosition").value;

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
        std::cout << "current object initial position : ";
        for(const double& t: object_initial_position)
            std::cout << t << " ";
        std::cout << std::endl;
    }

    float bSize[3] = {0.1f,0.1f,0.1f};
    object_handle = simCreatePureShape(2,0,bSize,0.05f,nullptr); //create a sphere as beacon;

    simSetObjectName(object_handle,"IRBeacon_0");
    const float tPos[3] = {static_cast<float>(object_initial_position[0]),
                     static_cast<float>(object_initial_position[1]),
                     static_cast<float>(object_initial_position[2])};

    if(simSetObjectPosition(object_handle,-1,tPos) < 0){
        std::cerr << "Set object position failed" << std::endl;
        exit(1);
    }
    simSetObjectSpecialProperty(object_handle,sim_objectspecialproperty_detectable_infrared);
//        simSetModelProperty(beacon_handle,sim_modelproperty_not_collidable | sim_modelproperty_not_dynamic);
    simSetObjectSpecialProperty(object_handle, sim_objectspecialproperty_collidable | sim_objectspecialproperty_measurable |
                                               sim_objectspecialproperty_detectable_all | sim_objectspecialproperty_renderable); // Detectable, collidable, etc.
    simSetObjectInt32Parameter(object_handle, sim_shapeintparam_respondable, 1);
    simComputeMassAndInertia(object_handle, 0.3f);
    float color[3] = {0.1f,1.0f,0.1f};
    simSetShapeColor(object_handle, nullptr, sim_colorcomponent_ambient_diffuse, color);

    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    trajectory.clear();
    object_trajectory.clear();

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }
    /// EB: This shouldn't be here!
    move_counter = 0;
}

std::vector<double> PushObject::fitnessFunction(const Individual::Ptr &ind){

    std::vector<double> d(1);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };


    double max_distance = settings::getParameter<settings::Double>(parameters,"#maxDistance").value;
    double distance_covered = distance(object_current_position,object_initial_position);

     d[0] = distance_covered/max_distance;

    return d;
}


float PushObject::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = std::dynamic_pointer_cast<sim::Morphology>(morph)->getMainHandle();

    waypoint wp;
    simGetObjectPosition(morphHandle, -1, wp.position);
    simGetObjectOrientation(morphHandle,-1,wp.orientation);


    if(fabs(final_position[0] - wp.position[0]) > 1e-3 ||
            fabs(final_position[1] - wp.position[1]) > 1e-3)
        move_counter++;

    final_position[0] = static_cast<double>(wp.position[0]);
    final_position[1] = static_cast<double>(wp.position[1]);
    final_position[2] = static_cast<double>(wp.position[2]);

    float interval = evalTime/static_cast<float>(nbr_wp);
    if(simulationTime >= interval*trajectory.size())
        trajectory.push_back(wp);

    simGetObjectPosition(object_handle, -1, wp.position);
    simGetObjectOrientation(object_handle,-1,wp.orientation);

    if(wp.is_nan())
        return 1;

    object_current_position[0] = static_cast<double>(wp.position[0]);
    object_current_position[1] = static_cast<double>(wp.position[1]);
    object_current_position[2] = static_cast<double>(wp.position[2]);

    if(simulationTime >= interval*object_trajectory.size())
        object_trajectory.push_back(wp);


    return 0;
}

void PushObject::build_tiled_floor(std::vector<int> &tiles_handles){
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
                height = (i+j)%2 == 0 ? -0.004 : 0.006;

//                height = randNum->randFloat(-0.005,-0.001);
            }
            float pos[3] = {starting_pos[0] + i*tile_increment,starting_pos[1] + j*tile_increment,height};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParameter(sim_bullet_body_friction,tiles_handles.back(),nullptr,1000);//randNum->randFloat(0,1000));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}
