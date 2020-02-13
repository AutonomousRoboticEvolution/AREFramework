#include "mazeEnv.h"
#include "v_repLib.h"

using namespace are;

void MazeEnv::init(){
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;

    Environment::init();

    target_position = {-0.575,0.575,0.05};

    //Generate small variations

//    simAddObjectToSelection(sim_handle_all,0);
//    int nbrObj = simGetObjectSelectionSize();
//    if(verbose)
//        std::cout << "Number of object selected : " << nbrObj << std::endl;

//    float variation_ratio = settings::getParameter<settings::Float>(parameters,"#variationRatio").value;
////    int nbrObj = 0;
//    int handles[nbrObj];
////    handles = simGetObjectsInTree(sim_handle_all,sim_object_shape_type,1,&nbrObj);

////    if(verbose)
////        std::cout << "Number of shapes found : " << nbrObj << std::endl;

//    simGetObjectSelection(handles);

//    float angles[3];
//    for(int i = 0; i < nbrObj; i++){
//        if(simGetObjectType(handles[i]) != sim_object_shape_type)
//            continue;
//        simGetObjectOrientation(handles[i],-1,angles);
//        angles[0] = variation_ratio*angles[0] + angles[0];
//        angles[1] = variation_ratio*angles[1] + angles[1];
//        angles[2] = variation_ratio*angles[2] + angles[2];
//        simSetObjectOrientation(handles[i],-1,angles);
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
