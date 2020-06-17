#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <vector>
#include <memory>
#include <sstream>
#include "misc/RandNum.h"
#include "ARE/Settings.h"

namespace are {

/// abstract class
class Control
{
public:

    typedef std::shared_ptr<Control> Ptr;
    typedef std::shared_ptr<const Control> ConstPtr;
    typedef Control::Ptr (Factory)(int,misc::RandNum::Ptr);

    Control(){}
    Control(const Control& ctrl) :
        randomNum(ctrl.randomNum)
    {}
    virtual ~Control(){
        parameters.reset();
        randomNum.reset();
    }

    virtual Control::Ptr clone() const = 0;

    /**
    @brief Take sensorInput and returns output
    @param sensorValues sensor values
    */
    virtual std::vector<double> update(const std::vector<double> &sensorValues) = 0;

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}

protected:
    misc::RandNum::Ptr randomNum;
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;
};

}//are

#endif //CONTROL_H
