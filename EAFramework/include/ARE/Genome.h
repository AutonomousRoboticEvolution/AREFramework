#pragma once
#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include "ARE/Settings.h"
#include "ARE/misc/RandNum.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace are {
class Genome
{
public:

    typedef std::shared_ptr<Genome> Ptr;
    typedef std::shared_ptr<const Genome> ConstPtr;
    typedef Genome::Ptr (Factory)(int, misc::RandNum::Ptr);

    Genome(int id = 0) : _id(id){}
    Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param, int id = 0);
    Genome(const Genome& gen) :
        parameters(gen.parameters),
        properties(gen.properties),
        randomNum(gen.randomNum),
        initialized(gen.initialized),
        type(gen.type),
        _id(gen._id)
    {}
    virtual ~Genome();

    /// This method deep copies the genome
    virtual Genome::Ptr clone() const = 0;

    /// Initialize the morph and its control
    virtual void init() = 0;

    virtual void mutate() = 0;

    virtual void crossover(const Genome::Ptr& partner,Genome::Ptr child1){
        child1 = clone();
    }

    virtual void symmetrical_crossover(const Genome::Ptr& partner,Genome::Ptr child1,Genome::Ptr child2){
        child1 = clone();
        child2 = partner->clone();
    }

    virtual void random(){}

    virtual std::string to_string() const = 0;
    virtual void from_string(const std::string&) = 0;
    virtual void from_file(const std::string&);

    //Getters & Setters
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    virtual void set_randNum(const misc::RandNum::Ptr& rn){randomNum = rn;}
    const std::string& get_type(){return type;}
    const int id() const {return _id;}
    void set_id(int id){_id = id;}


    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & _id;
        arch & type;
//        arch & initialized;
    }

protected:
    /// simulation setting shared by genome and EA
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;
    /// random number generator
    misc::RandNum::Ptr randomNum;

    bool initialized = false;
    std::string type;
    int _id;


};

/**
 * @brief The EmptyGenome class is meant to be used when no genome is needed.
 */
class EmptyGenome : public Genome
{
public:
    EmptyGenome() {type = "empty_genome";}
    Genome::Ptr clone() const{return nullptr;}
    void init(){}
    void mutate(){}
    std::string to_string() const override{return "";}
    void from_string(const std::string &) override{}

    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
    }
};


}//are

#endif //GENOME_H
