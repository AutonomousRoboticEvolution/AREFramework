#pragma once
#ifndef GENOME_H
#define GENOME_H

#include <vector>
#include <memory>
#include "ARE/Settings.h"
#include "misc/RandNum.h"
#include "ARE/Phenotype.h"

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

    virtual Phenotype::Ptr develop() = 0;

    virtual void mutate() = 0;

    //Getters & Setters
    void set_parameters(const settings::ParametersMapPtr &param){parameters = param;}
    const settings::ParametersMapPtr &get_parameters(){return parameters;}
    const settings::Property::Ptr &get_properties(){return properties;}
    void set_properties(const settings::Property::Ptr& prop){properties = prop;}
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
//        arch & initialized;
    }

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
    Phenotype::Ptr develop(){return nullptr;}
    void mutate(){}

//    template <class archive>
//    void serialize(archive &arch, const unsigned int v)
//    {
//    }


};


}//are

#endif //GENOME_H