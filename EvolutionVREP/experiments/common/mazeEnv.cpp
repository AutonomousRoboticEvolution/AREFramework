#include "mazeEnv.h"
#include "v_repLib.h"

#include <boost/algorithm/string.hpp>

using namespace are;

void MazeEnv::init(){

    randNum.setSeed(rand());

    Environment::init();


    target_position = {settings::getParameter<settings::Float>(parameters,"#target_x").value,
                       settings::getParameter<settings::Float>(parameters,"#target_y").value,
                       settings::getParameter<settings::Float>(parameters,"#target_z").value};

    //Generate small variations
    simAddObjectToSelection(sim_handle_all,0);
    int nbrObj = simGetObjectSelectionSize();

    float variation_ratio = settings::getParameter<settings::Float>(parameters,"#variationRatio").value;

    int handles[nbrObj];

    simGetObjectSelection(handles);

    float angles[3];
    for(int i = 0; i < nbrObj; i++){
        std::string objName = simGetObjectName(handles[i]);
        std::vector<std::string> strs;
        boost::split(strs,objName,boost::is_any_of("_"));
        if(strs[0] != "Wall")
            continue;
        simGetObjectOrientation(handles[i],-1,angles);
        angles[0] = randNum.randFloat(-variation_ratio,variation_ratio) + angles[0];
        angles[1] = angles[1];
        angles[2] = randNum.randFloat(-variation_ratio,variation_ratio) + angles[2];
        simSetObjectOrientation(handles[i],-1,angles);
    }

//    std::string model_path = settings::getParameter<settings::String>(parameters,"#modelPath").value;

//    for(int i = 0; i < 60*variation_ratio; i++){
//        std::string small_heap = model_path +std::string("/ellipsoid.ttm");
//        int handle = simLoadModel(small_heap.c_str());
//        if(handle < 0)
//            std::cerr << "unable to load model : " << model_path << "/small_heap.ttm" << std::endl;
//        float pos[3];
//        pos[0] = randNum.randFloat(-0.78,0.78);
//        pos[1] = randNum.randFloat(-0.78,0.78);
//        pos[2] = -0.02;
//        simSetObjectPosition(handle,-1,pos);
//    }
}

double MazeEnv::fitnessFunction(const Individual::Ptr &ind){
    auto distance = [](std::vector<double> a,std::vector<double> b) -> double
    {
        return std::sqrt((a[0] - b[0])*(a[0] - b[0]) +
                         (a[1] - b[1])*(a[1] - b[1]) +
                         (a[2] - b[2])*(a[2] - b[2]));
    };
    double d = distance(final_position,target_position);
    return -d;
}

float MazeEnv::updateEnv(float simulationTime, const Morphology::Ptr &morph){

    float timeStep = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    int morphHandle = morph->getMainHandle();

    float pos[3];
    simGetObjectPosition(morphHandle, -1, pos);


    final_position[0] = static_cast<double>(pos[0]);
    final_position[1] = static_cast<double>(pos[1]);
    final_position[2] = static_cast<double>(pos[2]);


    return 0;
}
