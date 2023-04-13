#ifndef CPPNGENOME_H
#define CPPNGENOME_H

#include <boost/serialization/export.hpp>
#include "ARE/Genome.h"
#include "ARE/morphology_descriptors.hpp"
#include "ARE/Logging.h"
#include <multineat/Genome.h>

namespace are {

/**
 * @brief CPPN genome using cppn network from the neat::Genome. Attended to be a morphogical genome to generate are::sim::Morphology_CPPNMatrix
 */
class CPPNGenome : public Genome
{
public:
    typedef std::shared_ptr<CPPNGenome> Ptr;
    typedef std::shared_ptr<const CPPNGenome> ConstPtr;

    /**
     * @brief default constructor
     */
    CPPNGenome() : Genome(){}
    /**
     * @brief constructor to build an empty cppn genome
     * @param random number generator (global to the experiment)
     * @param parameters (global to the experiment)
     * @param the id assigned to this genome
     */
    CPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param, int id = 0) :
        Genome(rn,param,id){}
    /**
     * @brief constructor to build from a neat::Genome
     * @param neat_gen
     */
    CPPNGenome(const NEAT::Genome &neat_gen) : neat_genome(neat_gen){}
    /**
     * @brief copy constructor
     * @param gen
     */
    CPPNGenome(const CPPNGenome &gen) :
        Genome(gen),neat_genome(gen.neat_genome){}
    ~CPPNGenome() override {}

    /**
     * @brief deep copy of the class
     * @return
     */
    Genome::Ptr clone() const override {
        return std::make_shared<CPPNGenome>(*this);
    }

    void init() override {}
    void mutate() override {}
    /**
     * @brief deserialization from a string
     * @param str
     */
    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<CPPNGenome>();
        iarch >> *this;
    }
    /**
     * @brief serialization of the class into a string
     */
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<CPPNGenome>();
        oarch << *this;
        return sstream.str();
    }

    /**
     * @brief get the neat::Genome
     * @return
     */
    const NEAT::Genome &get_neat_genome(){return neat_genome;}

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & neat_genome;
        arch & morpho_desc;
    }

    void set_morph_desc(const CartDesc& md){morpho_desc = md;}
    const CartDesc &get_morph_desc() const{return morpho_desc;}

    int get_nb_neurons() const {return neat_genome.NumNeurons();}
    int get_nb_connections() const {return neat_genome.NumLinks();}

private:
    NEAT::Genome neat_genome; //!< neat::Genome, actual implementation of the genome
    CartDesc morpho_desc; //!< descriptor of the morphology (phenotype)
};

namespace neat_cppn{

/**
 * @brief Logging class for the are::CPPNGenome
 */
class NbrConnNeurLog : public Logging
{
public:
    NbrConnNeurLog(const std::string& filename) : Logging(filename, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

}



}

#endif //CPPNGENOME_H

