#include "straightLineEnv.hpp"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include <boost/algorithm/string.hpp>
#include <simulatedER/Morphology.h>

using namespace are;
using namespace are::sim;

StraightLine::StraightLine(const settings::ParametersMapPtr& params)
{
    parameters = params;
    final_position.resize(3);
    name = "straight_line";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
    settings::defaults::parameters->emplace("#withTiles",new settings::Boolean(false));

}

void StraightLine::init(){

    VirtualEnvironment::init();

    bool with_tiles = settings::getParameter<settings::Boolean>(parameters,"#withTiles").value;

    trajectory.clear();

    if(with_tiles){
        std::vector<int> th;
        build_tiled_floor(th);
    }
    /// EB: This shouldn't be here!
    move_counter = 0;
}

std::vector<double> StraightLine::fitnessFunction(const Individual::Ptr &ind){

    std::vector<double> d(1);
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };

    //////////////////////////////////////////////////////////////////////////////
    /// Straight line fitness
    double angle = atan2(final_position[1], final_position[0]);
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    std::vector<double> lineTarget;
    double speedConstant = 1.0 / nbr_wp;
    double localError = 0;
    for(int i = 0; i < trajectory.size(); i++){
        lineTarget.push_back(cos(angle) * i * speedConstant);
        lineTarget.push_back(sin(angle) * i * speedConstant);
        lineTarget.push_back(final_position[2]); // This is the height of the robot
        std::vector<double> tempTarget{trajectory.at(i).position[0], trajectory.at(i).position[1], trajectory.at(i).position[2]};
//        std::cout << "Error: " << localError << std::endl;
        localError += distance(tempTarget, lineTarget);
        lineTarget.clear();
    }
     d[0] = 1 - localError/trajectory.size();    //////////////////////////////////////////////////////////////////////////////

    return d;
}


float StraightLine::updateEnv(float simulationTime, const Morphology::Ptr &morph){
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

    return 0;
}

void StraightLine::build_tiled_floor(std::vector<int> &tiles_handles){
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
