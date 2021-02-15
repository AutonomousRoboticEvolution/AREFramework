#include "straightLineEnv.hpp"
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
#include <boost/algorithm/string.hpp>

using namespace are;

StraightLine::StraightLine()
{
    final_position.resize(3);
    name = "StraightLine";

    // Definition of default values of the parameters.
    settings::defaults::parameters->emplace("#arenaSize",new settings::Double(2.));
    settings::defaults::parameters->emplace("#nbrWaypoints",new settings::Integer(2));
    settings::defaults::parameters->emplace("#flatFloor",new settings::Boolean(true));
}

void StraightLine::init(){

    Environment::init();

    bool flatFloor = settings::getParameter<settings::Boolean>(parameters,"#flatFloor").value;

    trajectory.clear();

    if(!flatFloor){
        std::vector<int> th;
        build_tiled_floor(th);
    }
    /// EB: This shouldn't be here!
    move_counter = 0;

    error = 0;
    timeStepCounter = 0;
}

std::vector<double> StraightLine::fitnessFunction(const Individual::Ptr &ind){

    std::vector<double> d(1);

    //////////////////////////////////////////////////////////////////////////////
    /// Straight line fitness
    d[0] = 1 - (error/300);
    //////////////////////////////////////////////////////////////////////////////

    return d;
}


float StraightLine::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    float evalTime = settings::getParameter<settings::Float>(parameters,"#maxEvalTime").value;
    int nbr_wp = settings::getParameter<settings::Integer>(parameters,"#nbrWaypoints").value;
    int morphHandle = morph->getMainHandle();

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
    ////////////////////////////////////////////////////////////////////////////////
    /// \todo EB: This is temporal code for the experiment in the journal October
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };


    //////////////////////////////////////////////////////////////////////////////
    /// Straight line fitness
    const double DISTANCECONSTANT = 1.0/300;
    std::vector<double> lineTarget{0.0, DISTANCECONSTANT*timeStepCounter-0.5,final_position[2]};
    error += distance(final_position,lineTarget);
    //std::cout << "Time: " << timeStepCounter << " Distance: " << DISTANCECONSTANT*timeStepCounter << " Error: " << error << std::endl;
    timeStepCounter++;
    //////////////////////////////////////////////////////////////////////////////
    return 0;
}

void StraightLine::build_tiled_floor(std::vector<int> &tiles_handles){
    float tile_size[3] = {0.25f,0.25f,0.01f};
    float starting_pos[3] = {-0.875f,-0.875f,0.005f};
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            tiles_handles.push_back(simCreatePureShape(0,8,tile_size,0.05f,nullptr));
            std::stringstream name;
            name << "tile_" << i << j;
            simSetObjectName(tiles_handles.back(),name.str().c_str());
            float pos[3] = {starting_pos[0] + i*tile_size[0],starting_pos[1] + j*tile_size[1],randNum->randFloat(-0.005,-0.001)};
            simSetObjectPosition(tiles_handles.back(),-1,pos);
            simSetEngineFloatParameter(sim_bullet_body_friction,tiles_handles.back(),nullptr,randNum->randFloat(0,1));
            simSetObjectSpecialProperty(tiles_handles.back(),sim_objectspecialproperty_detectable_ultrasonic);
            simSetModelProperty(tiles_handles.back(), sim_modelproperty_not_dynamic);
        }
    }
}
