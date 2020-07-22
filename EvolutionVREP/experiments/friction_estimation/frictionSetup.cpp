#include "frictionSetup.hpp"
<<<<<<< HEAD
#if defined (VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif
=======
#include "v_repLib.h"

>>>>>>> new experiment to estimate friction coefficient
#include <boost/algorithm/string.hpp>

using namespace are;

void CubeMorphology::loadModel(){
    std::cout << "load model" << std::endl;

    std::string path = settings::getParameter<settings::String>(parameters,"#robotPath").value;
    int modelHandle = simLoadModel(path.c_str());

    //sim::loadModel(instance_type,path_epuck_m.c_str(),client_id);
    if(modelHandle == -1)
    {
        std::cerr << "unable to load robot model" << std::endl;
        simChar* lastError = simGetLastError();
        std::cerr << "simGetLastError : " << lastError << std::endl;
        simReleaseBuffer(lastError);
        exit(1);
    }

    mainHandle = modelHandle;
}

void CubeMorphology::createAtPosition(float x, float y, float z){
    float pos[3] {x,y,z};
    simSetObjectPosition(mainHandle,-1,pos);
}

void FricInd::createMorphology(){
    float tiltAngle = settings::getParameter<settings::Float>(parameters,"#planeTiltAngle").value;


    morphology.reset(new CubeMorphology(parameters));
    std::dynamic_pointer_cast<CubeMorphology>(morphology)->loadModel();
    morphology->createAtPosition(-0.05,0.05,0.24);
    int base_handle = morphology->getMainHandle();
    float orient[3];
    simGetObjectOrientation(base_handle,-1,orient);
    orient[0] = tiltAngle;
    simSetObjectOrientation(base_handle,-1,orient);
    int nbrObj = 0;
    int* handles = simGetObjectsInTree(base_handle,sim_handle_all,1,&nbrObj);
    if(nbrObj == 0){
        std::cerr << "no object found" << std::endl;
        return;
    }
    std::vector<std::string> split_str;
    for(int i = 0; i < nbrObj; i++){
        std::string name(simGetObjectName(handles[i]));
        boost::split(split_str,name,boost::is_any_of("_"));
        if(split_str[0] != "P")
                continue;
        simSetEngineFloatParameter(sim_bullet_body_friction,handles[i],nullptr,fric_coeff);
        simSetEngineFloatParameter(sim_bullet_body_oldfriction,handles[i],nullptr,fric_coeff);

    }
    simReleaseBuffer((simChar*)(handles));
}

void FricEA::init(){

    friction_coeff = settings::getParameter<settings::Float>(parameters,"#startFricCoeff").value;

    EmptyGenome::Ptr gen(new EmptyGenome);
    FricInd::Ptr ind(new FricInd(gen,gen));
    ind->set_parameters(parameters);
    ind->set_randNum(randomNum);
    std::dynamic_pointer_cast<FricInd>(ind)->set_fric_coeff(friction_coeff);
    population.push_back(ind);
}

void FricEA::init_next_pop(){

    float fric_coeff_decr = settings::getParameter<settings::Float>(parameters,"#fricCoeffDecr").value;
    friction_coeff-=fric_coeff_decr;
    std::dynamic_pointer_cast<FricInd>(population[0])->set_fric_coeff(friction_coeff);
}

bool FricEA::is_finish(){
    return population[0]->getObjectives()[0] > 1e-4;
}

void FrictionSetUp::init(){

    initial_pos[0] = -0.05;
    initial_pos[1] = 0.05;
    initial_pos[2] = 0.24;

    Environment::init();

    float tiltAngle = settings::getParameter<settings::Float>(parameters,"#planeTiltAngle").value;

    int planeHandle = simGetObjectHandle("Plane");
    float orient[3];
    simGetObjectOrientation(planeHandle,-1,orient);
    orient[0] = tiltAngle;
    simSetObjectOrientation(planeHandle,-1,orient);
}

std::vector<double> FrictionSetUp::fitnessFunction(const Individual::Ptr &ind){
    std::vector<double> obj(1);
    obj[0] = fabs(robot_pos[0] - initial_pos[0])
            + fabs(robot_pos[1] - initial_pos[1]);
            + fabs(robot_pos[2] - initial_pos[2]);

    return obj;
}


float FrictionSetUp::updateEnv(float simulationTime, const Morphology::Ptr &morph){
    int handle = morph->getMainHandle();
    simGetObjectPosition(handle,-1,robot_pos);

}

void FricCoeffLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    savePopFile << generation << "," << static_cast<FricEA*>(ea.get())->get_friction_coeff() << std::endl;
    savePopFile.close();
}
