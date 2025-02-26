#pragma once
#ifndef CONTROL_H
#define CONTROL_H

#include <vector>
#include <memory>
#include <sstream>
#include "ARE/misc/RandNum.h"
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
    @brief Take senosry values and returns controls values.
    @param sensorValues sensor values
    */
    virtual std::vector<double> update(const std::vector<double> &sensorValues) = 0;

    //GETTERS & SETTERS
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    void set_random_number(const misc::RandNum::Ptr &rnd){randomNum = rnd;}

protected:
    misc::RandNum::Ptr randomNum;
    settings::ParametersMapPtr parameters;
};

}//are

#endif //CONTROL_H
