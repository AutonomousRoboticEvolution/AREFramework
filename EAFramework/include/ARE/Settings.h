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
#include <stdexcept>

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
    const bool& get_value() const{return value;}
    void fromString(const std::string& str){
        try{
        value = std::stoi(str);
        }catch(...){
            std::cerr << "settings::Boolean: stoi failed, the parameter with value: " << value << " is of wrong type" << std::endl;
        }
    }
    friend std::ostream & operator << (std::ostream &out, const Boolean &s){
        out << s.value;
        return out;
    }
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
    const int& get_value() const{return value;}
    void fromString(const std::string& str){
        try{
            value = std::stoi(str);
        }catch(...){
            std::cerr << "settings::Integer: stoi failed, the parameter with value: " << value << " is of wrong type" << std::endl;
        }
    }
    friend std::ostream & operator << (std::ostream &out, const Integer &s){
        out << s.value;
        return out;
    }
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
    const float& get_value() const{return value;}
    void fromString(const std::string& str){
        try{
            value = std::stof(str);
        }catch(...){
            std::cerr << "settings::Float: stof failed, the parameter with value: " << value << " is of wrong type" << std::endl;
        }
    }
    friend std::ostream & operator << (std::ostream &out, const Float &s){
        out << s.value;
        return out;
    }
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
    const double& get_value() const{return value;}
    void fromString(const std::string& str){
        try{
            value = std::stof(str);
        }catch(...){
            std::cerr << "settings::Double: stod failed, the parameter with value: " << value << " is of wrong type" << std::endl;
        }
    }
    friend std::ostream & operator << (std::ostream &out, const Double &s){
        out << s.value;
        return out;
    }
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
    const std::string& get_value() const{return value;}
    void fromString(const std::string& str){value = str;}
    friend std::ostream & operator << (std::ostream &out, const String &s){
        out << s.value;
        return out;
    }
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};


template<typename T>
class Sequence : public Type
{
public:
    Sequence(){name = "sequence";}
    Sequence(std::vector<T> s) : value(s) {name = "sequence";}
    std::vector<T> value;
    const std::vector<T>& get_value() const{return value;}
    void fromString(const std::string& str){
        std::cerr << "no implementation for this type of sequence." << std::endl;
    }
    friend std::ostream & operator << (std::ostream &out, const Sequence &s){
        for(const auto& val: s.value){
            out << val << ";";
        }
        return out;
    }
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

//TODO: implement try catch for the sequences

template<>
class Sequence<int> : public Type
{
public:
    Sequence(){name = "sequence_int";}
    Sequence(std::vector<int> s) : value(s) {name = "sequence_int";}
    std::vector<int> value;
    const std::vector<int>& get_value() const{return value;}
    void fromString(const std::string& str){
        std::vector<std::string> split_str;
        misc::split_line(str,";",split_str);
        for(const auto& s: split_str) value.push_back(std::stoi(s));
    }
    friend std::ostream & operator << (std::ostream &out, const Sequence &s){
        for(const auto& val: s.value){
            out << val << ";";
        }
        return out;
    }
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};
template<>
class Sequence<float> : public Type
{
public:
    Sequence(){name = "sequence_float";}
    Sequence(std::vector<float> s) : value(s) {name = "sequence_float";}
    std::vector<float> value;
    const std::vector<float>& get_value() const{return value;}
    void fromString(const std::string& str){
        std::vector<std::string> split_str;
        misc::split_line(str,";",split_str);
        for(const auto& s: split_str) value.push_back(std::stof(s));
    }
    friend std::ostream & operator << (std::ostream &out, const Sequence &s){
        for(const auto& val: s.value){
            out << val << ";";
        }
        return out;
    }
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

template<>
class Sequence<double> : public Type
{
public:
    Sequence(){name = "sequence_double";}
    Sequence(std::vector<double> s) : value(s) {name = "sequence_double";}
    std::vector<double> value;
    const std::vector<double>& get_value() const{return value;}
    void fromString(const std::string& str){
        std::vector<std::string> split_str;
        misc::split_line(str,";",split_str);
        for(const auto& s: split_str) value.push_back(std::stod(s));
    }
    friend std::ostream & operator << (std::ostream &out, const Sequence &s){
        for(const auto& val: s.value){
            out << val << ";";
        }
        return out;
    }
    template<class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & name;
        arch & value;
    }
};

template<>
class Sequence<std::string> : public Type
{
public:
    Sequence(){name = "sequence_string";}
    Sequence(std::vector<std::string> s) : value(s) {name = "sequence_string";}
    std::vector<std::string> value;
    const std::vector<std::string>& get_value() const{return value;}
    void fromString(const std::string& str){
        misc::split_line(str,";",value);
    }
    friend std::ostream & operator << (std::ostream &out, const Sequence &s){
        for(const auto& val: s.value){
            out << val << ";";
        }
        return out;
    }
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
//    try{
    if(params == nullptr){
        std::cerr << "are::settings::getParamter error: params is empty" << std::endl;
        return T();
    }
    if(params->find(name) == params->end()){
        std::cerr << "are::settings::getParamter error: Unable to find parameters " << name << " of type " << T().name << std::endl
                  << "You should define it in the parameters file." << std::endl;
        if(settings::defaults::parameters->find(name) == settings::defaults::parameters->end()){
            std::cerr << "No default value found" << std::endl;
            return T();
        }
        T res = *(cast<T>(defaults::parameters->at(name)));
        std::cerr << "Using default value : " << res << std::endl;
        return res;
    }

    if(cast<T>(params->at(name)).get() == nullptr){
        std::cerr << "are::settings::getParamter error: wrong type for parameter " << name << std::endl;
        return T();
    }

    return *(cast<T>(params->at(name)));
    //}
    //catch(std::exception& e){
    //    std::cerr << e.what() << std::endl << "Bad type for parameter " << name << std::endl;
    //    exit(1);
    //}

}

template<typename T>
T getParameter(const ParametersMap &params,const std::string& name)
{    
    
    //segvcatch::init_segv(&misc::handle_segv);
//    try{
    if(params.find(name) == params.end()){
        std::cerr << "Unable to find parameters " << name << " of type " << T().name << std::endl
                  << "You should define it in the parameters file." << std::endl;
        if(settings::defaults::parameters->find(name) == settings::defaults::parameters->end()){
            std::cerr << "No default value found" << std::endl;
            return T();
        }
        T res = *(cast<T>(defaults::parameters->at(name)));
        std::cerr << "Using default value : " << res << std::endl;
        return res;
    }

    if(cast<T>(params.at(name)).get() == nullptr){
        std::cerr << "are::settings::getParamter error: wrong type for parameter " << name << std::endl;
    }
    return *(cast<T>(params.at(name)));
   // }
//    catch(std::exception& e){
//        std::cerr << e.what() << std::endl << "Bad type for parameter " << name << std::endl;
//        exit(1);
//    }
}


std::string toString(const ParametersMap& params);
ParametersMap fromString(const std::string& str_params);

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
    POS_TRAJ = 2,
    OBJ = 3
}obsType;

typedef enum jointCtrlType {
    DIRECT = 0,
    PROPORTIONAL = 1,
    OSCILLATORY = 2
}jointCtrlType;

} //settings

}//are

#endif //SETTINGS_H
