#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <fstream> // ifstream
#include <list>
#include <vector>
#include <misc/utilities.h>

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

template<typename T>
T getParameter(const ParametersMapPtr &params,const std::string& name)
{
    if(params->find(name) == params->end()){
        std::cerr << "unable to find parameters " << name << std::endl
                  << "you should define it in the parameters file." << std::endl;
        return T();
    }
    return *(cast<T>(params->at(name)));
}

template<typename T>
T getParameter(const ParametersMap &params,const std::string& name)
{
    if(params.find(name) == params.end()){
        std::cerr << "unable to find parameters " << name << std::endl
                  << "you should define it in the parameters file." << std::endl;
        return T();
    }
    return *(cast<T>(params.at(name)));
}

ParametersMap loadParameters(const std::string& file);
void saveParameters(const std::string& file,const ParametersMapPtr &param); //todo

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


} //settings

}//are

#endif //SETTINGS_H
