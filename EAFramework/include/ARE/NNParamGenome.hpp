#ifndef NN_PARAM_GENOME_HPP
#define NN_PARAM_GENOME_HPP

#include "ARE/Genome.h"
#include <boost/random.hpp>
#include <boost/algorithm/string.hpp>
#include "ARE/Logging.h"
#include "ARE/genetic_operators.hpp"

namespace are{



class NNParamGenome : public Genome
{
public:
    typedef std::shared_ptr<NNParamGenome> Ptr;
    typedef std::shared_ptr<const NNParamGenome> ConstPtr;


    NNParamGenome() : Genome() {type = "nn_param_genome";}
    NNParamGenome(misc::RandNum::Ptr rn, settings::ParametersMapPtr param, int id = 0) :
        Genome(rn,param,id){type = "nn_param_genome";}
    NNParamGenome(const NNParamGenome &ngen) :
        Genome(ngen), weights(ngen.weights), biases(ngen.biases),
        nbr_input(ngen.nbr_input),nbr_output(ngen.nbr_output),
        nbr_hidden(ngen.nbr_hidden), nn_type(ngen.nn_type){}

    Genome::Ptr clone() const override {
        return std::make_shared<NNParamGenome>(*this);
    }


    void init() override{

    }
    void mutate() override;
    void crossover(const Genome::Ptr &partner, Genome::Ptr child) override;
    void symmetrical_crossover(const Genome::Ptr &partner, Genome::Ptr child1, Genome::Ptr child2) override;


    void set_weights(const std::vector<double>& w){weights = w;}
    const std::vector<double>& get_weights() const {return weights;}

    void set_biases(const std::vector<double>& b){biases = b;}
    const std::vector<double>& get_biases() const {return biases;}

    void set_nbr_input(int ni){nbr_input = ni;}
    void set_nbr_output(int no){nbr_output = no;}
    void set_nbr_hidden(int nh){nbr_hidden = nh;}
    void set_nn_type(int nnt){nn_type = nnt;}

    const int get_nbr_input() const {return nbr_input;}
    const int get_nbr_output() const {return nbr_output;}
    const int get_nbr_hidden() const {return nbr_hidden;}
    const int get_nn_type() const {return nn_type;}

    std::vector<double> get_full_genome(){
        std::vector<double> genome = weights;
        genome.insert(genome.end(),biases.begin(),biases.end());
        return genome;
    }

    /**
     * @brief return the weights and biases into a string into the following format :
     *          nbr_weights
     *          nbr_bias
     *          w1
     *          w2
     *          ...
     *          wn
     *          ...
     *          b1
     *          b2
     *          ...
     *          bn
     *          ...
     * @return
     */
    std::string to_string() const override;
    void from_string(const std::string & gen_str) override;

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & weights;
        arch & biases;
        arch & nbr_input;
        arch & nbr_output;
        arch & nbr_hidden;
        arch & nn_type;
    }

private:
    std::vector<double> weights;
    std::vector<double> biases;
    int nbr_input=0;
    int nbr_output=0;
    int nbr_hidden=0;
    int nn_type=0;
};

class NNParamGenomeLog : public Logging
{
public:
    NNParamGenomeLog() : Logging(true){} //Logging at the end of the generation
    NNParamGenomeLog(double ot) :
        objective_threshold(ot),
        Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
private:
    double objective_threshold = 0;
};

}

#endif //NN_PARAM_GENOME_HPP
