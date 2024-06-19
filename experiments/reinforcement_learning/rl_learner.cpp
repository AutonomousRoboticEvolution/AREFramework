#include "rl_learner.hpp"

using namespace are;

void RLLearner::init(int id){

    smarties::MDPdescriptor desc;
    std::shared_ptr<smarties::ExecutionInfo> exec_info;

    _learner = smarties::createLearner(id,desc,*exec_info);
    _learner->restart();
//    _learner->setupTasks(algoTasks);
//    _learner->setupDataCollectionTasks(dataTask);

}

void RLLearner::set_state_action_dims(int state, int action){

}
