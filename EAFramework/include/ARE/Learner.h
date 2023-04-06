#ifndef LEARNER_H
#define LEARNER_H

#include <memory>
#include "ARE/Settings.h"
#include "ARE/Control.h"

namespace are{
/**
 * @brief The Learner class: Abstract class for the "learners" of the framework.
 * A learner is an optimizer for the controller of a robot.
 * It will be used either with a fixed morphology robot or nested in a morho-generative algorithm
 */
class Learner
{
public:

    typedef std::shared_ptr<Learner> Ptr;
    typedef std::shared_ptr<const Learner> ConstPtr;


    Learner(){}
    Learner(const Learner& l): parameters(l.parameters){}
    virtual ~Learner(){
        parameters.reset();
    }

    /**
     * @brief update the parameters of the controller in argument.
     * @param updated controller
     */
    virtual void update(Control::Ptr& ctrl) = 0;

    //GETTERS & SETTERS
    /**
     * @brief set the parameters
     * @param global hyper-parameters of the experiment
     */
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    /**
     * @brief get the parameters contain in this class
     * @return global hyper-parameters of the experiment
     */
    const settings::ParametersMapPtr &get_parameters(){return parameters;}


    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
    }

protected:
    settings::ParametersMapPtr parameters;

};
}//are
#endif //LEARNER_H
