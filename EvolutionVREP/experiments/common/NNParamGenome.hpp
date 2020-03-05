#ifndef NN_PARAM_GENOME_HPP
#define NN_PARAM_GENOME_HPP

#include <ARE/Genome.h>

namespace are{

class NNParamGenome : public Genome
{
public:
    typedef std::shared_ptr<NNParamGenome> Ptr;
    typedef std::shared_ptr<const NNParamGenome> ConstPtr;


    NNParamGenome() : Genome() {}
    NNParamGenome(misc::RandNum::Ptr rn, settings::ParametersMapPtr param) :
        Genome(rn,param){
    }
    NNParamGenome(const NNParamGenome &ngen) :
        Genome(ngen), weights(ngen.weights), biases(ngen.biases){}

    Genome::Ptr clone() const override {
        return std::make_shared<NNParamGenome>(*this);
    }


    void init() override{}
    Phenotype::Ptr develop() override{}
    void mutate() override{}


    void set_weights(const std::vector<double>& w){weights = w;}
    const std::vector<double>& get_weights(){return weights;}

    void set_biases(const std::vector<double>& b){biases = b;}
    const std::vector<double>& get_biases(){return biases;}

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
    std::string to_string() const {
        std::stringstream sstr;
        sstr << weights.size() << std::endl;
        sstr << biases.size() << std::endl;
        for(double w : weights)
            sstr << w << std::endl;
        for(double b : biases)
            sstr << b << std::endl;
        return sstr.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & weights;
        arch & biases;
    }

private:
    std::vector<double> weights;
    std::vector<double> biases;
};
}

#endif //NN_PARAM_GENOME_HPP
