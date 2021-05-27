#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <fstream> // ifstream
#include <sstream>
#include <list>
#include <vector>
#include "misc/utilities.h"

namespace are {

namespace settings
{

class Type
{
public:
    Type(){}
    virtual ~Type(){}
    typedef std::shared_ptr<Type> Ptr;
    typedef std::shared_ptr<const Type> ConstPtr;
    virtual void fromString(const std::string& str) = 0;
    std::string name;
};

class Boolean : public Type
{
public:
    Boolean(){name = "bool";}
    Boolean(bool b) : value(b){name = "bool";}
    bool value = false;
    void fromString(const std::string& str){value = std::stoi(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

class Integer : public Type
{
public:
    Integer(){name = "int";}
    Integer(int i) : value(i) {name = "int";}
    int value = 0;
    void fromString(const std::string& str){value = std::stoi(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

class Float : public Type
{
public:
    Float(){name = "float";}
    Float(float f) : value(f) {name = "float";}
    float value = 0.0;
    void fromString(const std::string& str){value = std::stof(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};


class Double : public Type
{
public:
    Double(){name = "double";}
    Double(double d) : value(d) {name = "double";}
    double value = 0.0;
    void fromString(const std::string& str){value = std::stod(str);}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

class String : public Type
{
public:
    String(){name = "string";}
    String(std::string s) : value(s) {name = "string";}
    std::string value = "";
    void fromString(const std::string& str){value = str;}
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};




Type::Ptr buildType(const std::string &name);

std::string toString(const std::string &name, const Type::ConstPtr &element);

template<typename T>
std::shared_ptr<const T> cast(const Type::ConstPtr val)
{
    return std::dynamic_pointer_cast<const T>(val);
}

/// Instance type defines in what mode the "Evolutionary Robotics" plugin runs // TODO: define name
enum InstanceType {
    /// Single thread (local)
    INSTANCE_REGULAR = 0,
    /// Waits for genome signals for using in parallel execution
    INSTANCE_SERVER = 1,
    /// deprecated
    INSTANCE_DEBUGGING = 2
};

typedef std::map<const std::string,const Type::ConstPtr> ParametersMap;
typedef std::shared_ptr<ParametersMap> ParametersMapPtr;

/**
 * @brief Default parameters.
 */
struct defaults{
    static ParametersMapPtr parameters;
};

/**
 * @brief Randomly generated parameters.
 */
struct random{
    static ParametersMapPtr parameters;
};

template<typename T>
T getParameter(const ParametersMapPtr &params,const std::string& name)
{
    if(params->find(name) == params->end()){
        std::cerr << "Unable to find parameters " << name << " of type " << T().name << std::endl
                  << "You should define it in the parameters file." << std::endl;
        if(settings::defaults::parameters->find(name) == settings::defaults::parameters->end()){
            std::cerr << "No default value found" << std::endl;
            return T();
        }
        T res = *(cast<T>(defaults::parameters->at(name)));
        std::cerr << "Using default value : " << res.value << std::endl;
        return res;
    }
    return *(cast<T>(params->at(name)));
}

template<typename T>
T getParameter(const ParametersMap &params,const std::string& name)
{
    if(params.find(name) == params.end()){
        std::cerr << "Unable to find parameters " << name << " of type " << T().name << std::endl
                  << "You should define it in the parameters file." << std::endl;
        if(settings::defaults::parameters->find(name) == settings::defaults::parameters->end()){
            std::cerr << "No default value found" << std::endl;
            return T();
        }
        T res = *(cast<T>(defaults::parameters->at(name)));
        std::cerr << "Using default value : " << res.value << std::endl;
        return res;
    }
    return *(cast<T>(params.at(name)));
}

/**
 * @brief Load the parameters from a file. No need to call it explicitly. It is called at the start of the plugin, either in v_repExtER or in ERClient.
 * @param parameter file in csv format
 * @return the parameters map
 */
ParametersMap loadParameters(const std::string& file);

/**
 * @brief Save the parameters in the log folder
 * @param file
 * @param param
 */
void saveParameters(const std::string& file,const ParametersMapPtr &param);

//To re-evaluate, likely to be removed
struct Property
{
    typedef std::shared_ptr<Property> Ptr;
    typedef std::shared_ptr<const Property> ConstPtr;

    Property(){}
    Property(const Property& prop) :
        generation(prop.generation)
    {}

    //Properties
    int generation = 0;
    std::vector<int> indNumbers;
    std::vector<double> indFits;
    int indCounter = 0;
    int clientID;
};


enum genomeType {
    NEAT = 0,
    NN = 1,
    NNPARAM = 2
};

typedef enum obsType {
    FINAL_POS = 0,
    TRAJECTORY = 1,
    POS_TRAJ = 2
}obsType;

typedef enum jointCtrlType {
    DIRECT = 0,
    PROPORTIONAL = 1,
    OSCILLATORY = 2
}jointCtrlType;

} //settings

}//are

#endif //SETTINGS_H
