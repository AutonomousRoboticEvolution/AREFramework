#ifndef RL_LEARNER_HPP
#define RL_LEARNER_HPP

#include "smarties.h"
#include "smarties/Learners/AlgoFactory.h"


namespace are {

class RLLearner
{
public:
    RLLearner(){}
    void init(int id = 0);

    void set_state_action_dims(int state, int action);
    void set_action_scales(const std::vector<double> &upper,const std::vector<double> &lower, bool bounded = true);

private:
    std::unique_ptr<smarties::Learner> _learner;
};

}

#endif //RL_LEARNER_HPP
