#pragma once
#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include "ARE/Settings.h"
#include "misc/RandNum.h"

#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace are {
class Genome
{
public:

    typedef std::shared_ptr<Genome> Ptr;
    typedef std::shared_ptr<const Genome> ConstPtr;
    typedef Genome::Ptr (Factory)(int, misc::RandNum::Ptr);

    Genome(){}
    Genome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param);
    Genome(const Genome& gen) :
        parameters(gen.parameters),
        properties(gen.properties),
        randomNum(gen.randomNum),
        initialized(gen.initialized)
    {}
    virtual ~Genome();

    /// This method deep copies the genome
    virtual Genome::Ptr clone() const = 0;

    /// Initialize the morph and its control
    virtual void init() = 0;

    virtual void mutate() = 0;

    virtual void crossover(const Genome::Ptr& partner,Genome::Ptr child1,Genome::Ptr child2){
        child1 = partner->clone();
        child2 = clone();
    }
    /// for saving genome
    virtual std::string to_string() const = 0;
    virtual void from_string(const std::string&) = 0;

    //Getters & Setters
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    void set_randNum(const misc::RandNum::Ptr& rn){randomNum = rn;}


    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
//        arch & initialized;
    }

    void saveGenome(FILE *file){};

protected:
    /// simulation setting shared by genome and EA
    settings::ParametersMapPtr parameters;
    settings::Property::Ptr properties;
    /// random number generator
    misc::RandNum::Ptr randomNum;

    bool initialized = false;
};

/**
 * @brief The EmptyGenome class is meant to be used when no genome is needed.
 */
class EmptyGenome : public Genome
{
public:
    Genome::Ptr clone() const{return nullptr;}
    void init(){}
    void mutate(){}
    std::string to_string() const override{return "";}
    void from_string(const std::string &) override{}

//    template <class archive>
//    void serialize(archive &arch, const unsigned int v)
//    {
//    }


};


}//are

#endif //GENOME_H
