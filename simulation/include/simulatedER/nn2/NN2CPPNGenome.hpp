#ifndef NN2CPPNGENOME_HPP
#define NN2CPPNGENOME_HPP

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "nn2/cppn.hpp"
#include "ARE/Genome.h"
#include "ARE/Logging.h"
#include "simulatedER/morphology_descriptors.hpp"
#include "simulatedER/skeleton_generation.hpp"

namespace are {

struct cppn_params{
    struct cppn{
        static int _mutation_type;
        static bool _mutate_connections;
        static bool _mutate_neurons;
        static float _mutation_rate;
        static float _rate_mutate_conn;
        static float _rate_mutate_neur;
        static float _rate_add_neuron;
        static float _rate_del_neuron;
        static float _rate_add_conn;
        static float _rate_del_conn;
        static float _rate_change_conn;
        static float _rate_crossover;

        static size_t _min_nb_neurons;
        static size_t _max_nb_neurons;
        static size_t _min_nb_conns;
        static size_t _max_nb_conns;

        static int nb_inputs;
        static int nb_outputs;
    };
    struct evo_float{
        static float mutation_rate;
        static constexpr float cross_rate = 0.f;
        static constexpr nn2::evo_float::mutation_t mutation_type = nn2::evo_float::gaussian;
        static constexpr nn2::evo_float::cross_over_t cross_over_type = nn2::evo_float::no_cross_over;
        static constexpr float eta_m = 15.f; //parameter for polynomial mutation
        static constexpr float eta_c = 15.f; //parameter for polynomial mutation
        static constexpr float sigma = 0.5; //parameter for gaussian mutation
        static constexpr float min = -1;
        static constexpr float max = 1;
    };
};


namespace cppn {
using af_t = nn2::AfCppn<nn2::cppn::AfParams<cppn_params>>;
using pf_t = nn2::PfWSum<nn2::EvoFloat<1,cppn_params>>;
using neuron_t = nn2::Neuron<pf_t,af_t>;
using connection_t = nn2::Connection<nn2::EvoFloat<1,cppn_params>>;
}

using nn2_cppn_t = nn2::CPPN<cppn::neuron_t,cppn::connection_t,cppn_params>;




class NN2CPPNGenome : public Genome
{
public:


    typedef std::shared_ptr<NN2CPPNGenome> Ptr;
    typedef std::shared_ptr<const NN2CPPNGenome> ConstPtr;

    NN2CPPNGenome() : Genome(){
        cppn = nn2_cppn_t(cppn_params::cppn::nb_inputs,cppn_params::cppn::nb_outputs);
        type = "nn2_cppn_genome";
    }
    NN2CPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
        cppn = nn2_cppn_t(cppn_params::cppn::nb_inputs,cppn_params::cppn::nb_outputs);
        type = "nn2_cppn_genome";

    }
    NN2CPPNGenome(const nn2_cppn_t &nn2_cppn_gen) : cppn(nn2_cppn_gen){
    }
    NN2CPPNGenome(const NN2CPPNGenome &gen) :
        Genome(gen), cppn(gen.cppn), feat_desc(gen.feat_desc), organ_position_desc(gen.organ_position_desc), matrix_desc(gen.matrix_desc){
    }
    ~NN2CPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<NN2CPPNGenome>(*this);
    }

    void init() override {
        cppn.init();
    }

    void fixed_structure(){
        cppn.build_fixed_structure();
    }

    void random() override{
        cppn.random();
    }

    void mutate() override {
        cppn.mutate();
    }

    void crossover(const Genome::Ptr &partner,Genome::Ptr child) override {
        are::nn2_cppn_t cppn_child;
        are::nn2_cppn_t cppn_partner = std::dynamic_pointer_cast<NN2CPPNGenome>(partner)->get_cppn();
        cppn.crossover(cppn_partner,cppn_child);
        *std::dynamic_pointer_cast<NN2CPPNGenome>(child) = NN2CPPNGenome(cppn_child);
        child->set_parameters(parameters);
        child->set_randNum(randomNum);
        child->set_parents_ids({_id,partner->id()});
    }

    void symmetrical_crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override{
        are::nn2_cppn_t cppn_child1, cppn_child2;
        are::nn2_cppn_t cppn_partner = std::dynamic_pointer_cast<NN2CPPNGenome>(partner)->get_cppn();
        cppn.crossover(cppn_partner,cppn_child1);
        cppn_partner.crossover(cppn,cppn_child2);
	    *std::dynamic_pointer_cast<NN2CPPNGenome>(child1) = NN2CPPNGenome(cppn_child1);
	    *std::dynamic_pointer_cast<NN2CPPNGenome>(child2) = NN2CPPNGenome(cppn_child2);
        child1->set_parameters(parameters);
        child2->set_parameters(parameters);
        child1->set_randNum(randomNum);
        child2->set_randNum(randomNum);
        child1->set_parents_ids({_id,partner->id()});
        child2->set_parents_ids({partner->id(),_id});
    }

    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<NN2CPPNGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<NN2CPPNGenome>();
        oarch << *this;
        return sstream.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & cppn;
        arch & feat_desc;
        arch & organ_position_desc;
        arch & matrix_4d;
        arch & generation;
    }

    const sim::FeaturesDesc& get_feat_desc() const {return feat_desc;}
    void set_feature_desc(const sim::FeaturesDesc& md){feat_desc = md;}

    const sim::OrganMatrixDesc& get_organ_position_desc() const {return organ_position_desc;}
    void set_organ_position_desc(const sim::OrganMatrixDesc& opd){organ_position_desc = opd;}

    const sim::MatrixDesc& get_matrix_desc() const {return matrix_desc;}
    void set_matrix_desc(const sim::MatrixDesc& mat){matrix_desc = mat;}

    void set_cppn(const nn2_cppn_t &c){cppn = c;}
    const nn2_cppn_t& get_cppn() const {return cppn;}
    void set_matrix_4d(const std::vector<std::vector<double>> m4d){matrix_4d = m4d;}
    const std::vector<std::vector<double>>& get_matrix_4d(){return matrix_4d;}

    int get_nb_neurons(){return cppn.get_nb_neurons();}
    int get_nb_connections(){return cppn.get_nb_connections();}

    int get_generation(){return generation;}
    void incr_generation(){generation++;}

    void set_randNum(const misc::RandNum::Ptr& rn) override{
        randomNum = rn;
        //nn2::rgen_t::gen.seed(randomNum->getSeed());
    }

private:
    nn2_cppn_t cppn;
    sim::FeaturesDesc feat_desc;
    sim::OrganMatrixDesc organ_position_desc;
    sim::MatrixDesc matrix_desc;
    int generation=0;
    std::vector<std::vector<double>> matrix_4d;
};

namespace  nn2_cppn_decoder {

using namespace sim::organ;

void decode(nn2_cppn_t &cppn,
            skeleton::type& skeleton,
            organ_list_t &organ_list,
            int &number_voxels,
            bool growing = false);
void generate_skeleton(nn2_cppn_t &cppn,
                       skeleton::type& skeleton);
void generate_skeleton_growing(nn2_cppn_t &cppn,
                       skeleton::type& skeleton);
void generate_organ_list(nn2_cppn_t &cppn,
                         const skeleton::coord_t &surface_coords,
                         int nbr_organs,
                         organ_list_t &organ_list);
int cppn_to_organ_type(nn2_cppn_t &cppn,const std::vector<double> &input);
};

namespace nn2_cppn {

class NN2CPPNGenomeLog : public Logging
{
public:
    NN2CPPNGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class NbrConnNeurLog : public Logging
{
public:
    NbrConnNeurLog(const std::string& filename) : Logging(filename, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class ParentingLog : public Logging
{
public:
    ParentingLog(const std::string& filename) : Logging(filename, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class GraphVizLog : public Logging
{
public:
    GraphVizLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

}//nn2_cppn
}//are

#endif //NN2CPPNGENOME_HPP

