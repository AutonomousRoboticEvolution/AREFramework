#ifndef PROTOMATRIXGENOME_HPP
#define PROTOMATRIXGENOME_HPP

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "nn2/cppn.hpp"
#include "ARE/Genome.h"
#include "ARE/morphology_descriptors.hpp"
#include "ARE/Logging.h"
#include "protomatrix_utils.hpp"

namespace are {

//struct cppn_params{
//    struct cppn{
//        static int _mutation_type;
//        static bool _mutate_connections;
//        static bool _mutate_neurons;
//        static float _mutation_rate;
//        static float _rate_mutate_conn;
//        static float _rate_mutate_neur;
//        static float _rate_add_neuron;
//        static float _rate_del_neuron;
//        static float _rate_add_conn;
//        static float _rate_del_conn;
//        static float _rate_change_conn;
//        static float _rate_crossover;
//
//        static size_t _min_nb_neurons;
//        static size_t _max_nb_neurons;
//        static size_t _min_nb_conns;
//        static size_t _max_nb_conns;
//
//        static int nb_inputs;
//        static int nb_outputs;
//    };
//    struct evo_float{
//        static float mutation_rate;
//        static constexpr float cross_rate = 0.f;
//        static constexpr nn2::evo_float::mutation_t mutation_type = nn2::evo_float::polynomial;
//        static constexpr nn2::evo_float::cross_over_t cross_over_type = nn2::evo_float::no_cross_over;
//        static constexpr float eta_m = 15.f;
//        static constexpr float eta_c = 15.f;
//        static constexpr float min = -1;
//        static constexpr float max = 1;
//    };
//};


//namespace cppn {
//using af_t = nn2::AfCppn<nn2::cppn::AfParams<cppn_params>>;
//using pf_t = nn2::PfWSum<nn2::EvoFloat<1,cppn_params>>;
//using neuron_t = nn2::Neuron<pf_t,af_t>;
//using connection_t = nn2::Connection<nn2::EvoFloat<1,cppn_params>>;
//}

//using nn2_cppn_t = nn2::CPPN<cppn::neuron_t,cppn::connection_t,cppn_params>;

//static int static_id;
static int protomatrix_id; /// \todo EB: Why can't I use static_id

class ProtomatrixGenome : public Genome
{
public:


    typedef std::shared_ptr<ProtomatrixGenome> Ptr;
    typedef std::shared_ptr<const ProtomatrixGenome> ConstPtr;

    ProtomatrixGenome() : Genome(){
//        _id = static_id++;
        _id = protomatrix_id++;
//        cppn = nn2_cppn_t(cppn_params::cppn::nb_inputs,cppn_params::cppn::nb_outputs);
        type = "protomatrix_genome";
        parents_ids= std::vector<int>(2,-1);
    }
    ProtomatrixGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
//        _id = static_id++;
        _id = protomatrix_id++;
//        cppn = nn2_cppn_t(cppn_params::cppn::nb_inputs,cppn_params::cppn::nb_outputs);
        type = "protomatrix_genome";
        parents_ids= std::vector<int>(2,-1);
    }
//    ProtomatrixGenome(const nn2_cppn_t &nn2_cppn_gen) : cppn(nn2_cppn_gen){
//        _id = static_id++;
//        _id = protomatrix_id++;
//    }
    ProtomatrixGenome(const ProtomatrixGenome &gen) :
        Genome(gen), matrix_4d(gen.matrix_4d), morph_desc(gen.morph_desc), parents_ids(gen.parents_ids){
    }
    ~ProtomatrixGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<ProtomatrixGenome>(*this);
    }

    void init() override {

    }

    void fixed_structure(){
//        cppn.build_fixed_structure();
    }

    void random(){
//        cppn.random();
        if(settings::getParameter<settings::Boolean>(parameters,"#isRandomStartingPopulation").value) {
            matrix_4d = protomatrix::random_matrix(matrix_4d);
        }
        else{
            std::string genome_pool = settings::getParameter<settings::String>(parameters,"#genomePool").value;
            std::string genome_id = std::to_string(id());
            std::string temp_string = genome_pool + genome_id;
            matrix_4d = protomatrix::load_robot_matrix(temp_string);
        }
    }

    void mutate() override {
//        cppn.mutate();
        matrix_4d = protomatrix::mutate_matrix(matrix_4d);
    }

    void crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override {
//        are::nn2_cppn_t cppn_child1, cppn_child2;
//        are::nn2_cppn_t cppn_partner = std::dynamic_pointer_cast<ProtomatrixGenome>(partner)->get_cppn();
//        cppn.crossover(cppn_partner,cppn_child1);
//        cppn_partner.crossover(cppn,cppn_child2);
//        *child1 = ProtomatrixGenome(cppn_child1);
//        *child2 = ProtomatrixGenome(cppn_child2);
//        child1->set_parameters(parameters);
//        child2->set_parameters(parameters);
//        child1->set_randNum(randomNum);
//        child2->set_randNum(randomNum);
//        std::dynamic_pointer_cast<ProtomatrixGenome>(child1)->set_parents_ids({_id,partner->id()});
//        std::dynamic_pointer_cast<ProtomatrixGenome>(child2)->set_parents_ids({_id,partner->id()});
    }


    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<ProtomatrixGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<ProtomatrixGenome>();
        oarch << *this;
        return sstream.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
//        arch & cppn;
        arch & morph_desc;
        arch & parents_ids;
        arch & matrix_4d;
        arch & generation;
    }

    const CartDesc& get_morph_desc() const {return morph_desc;}
    void set_morph_desc(const CartDesc& md){morph_desc = md;}

    const std::vector<int>& get_parents_ids() const {return parents_ids;}
    void set_parents_ids(const std::vector<int>& ids){parents_ids = ids;}
    void set_matrix_4d(const std::vector<std::vector<double>> m4d){matrix_4d = m4d;}
    const std::vector<std::vector<double>>& get_matrix_4d(){return matrix_4d;}

//    int get_nb_neurons(){return cppn.get_nb_neurons();}
//    int get_nb_connections(){return cppn.get_nb_connections();}

    int get_generation(){return generation;}
    void incr_generation(){generation++;}

    void set_randNum(const misc::RandNum::Ptr& rn) override{
        randomNum = rn;
        //nn2::rgen_t::gen.seed(randomNum->getSeed());
    }

private:
    std::vector<int> parents_ids;
//    nn2_cppn_t cppn;
    CartDesc morph_desc;
    int generation;
    std::vector<std::vector<double>> matrix_4d;
};

namespace protomatrix {

class ProtomatrixGenomeLog : public Logging
{
public:
    ProtomatrixGenomeLog() : Logging(true){} //Logging at the end of the generation
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

}//protomatrix
}//are

#endif //PROTOMATRIXGENOME_HPP

