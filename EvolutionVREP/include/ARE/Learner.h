#ifndef LEARNER_H
#define LEARNER_H

#include <memory>
#include "ARE/Settings.h"
#include "ARE/Control.h"

namespace are{
class Learner
{
public:

    typedef std::shared_ptr<Learner> Ptr;
    typedef std::shared_ptr<const Learner> ConstPtr;

    Learner(){}
    virtual ~Learner(){
        parameters.reset();
    }

    virtual void update(const Control::Ptr&) = 0;

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}

protected:
    settings::ParametersMapPtr parameters;

};
}//are
#endif //LEARNER_H
