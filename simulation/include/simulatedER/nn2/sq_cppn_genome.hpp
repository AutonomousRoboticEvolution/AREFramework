#ifndef SQ_CPPN_GENOME_HPP
#define SQ_CPPN_GENOME_HPP

#include "ARE/Genome.h"
#include "ARE/misc/utilities.h"
#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <vector>
#include "nn2/cppn.hpp"
#include "ARE/genetic_operators.hpp"
#include "ARE/Logging.h"
#include "ARE/quadrics.hpp"
#include "simulatedER/skeleton_generation.hpp"
#include "simulatedER/Organ.h"
#include "simulatedER/morphology_descriptors.hpp"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace are{

namespace sq_cppn{
struct params{
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

        static constexpr int nb_inputs = 3;
        static constexpr int nb_outputs = 1;

        static double _expressiveness;
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

using af_t = nn2::AfCppn<nn2::cppn::AfParams<params>>;
using pf_t = nn2::PfWSum<nn2::EvoFloat<1,params>>;
using neuron_t = nn2::Neuron<pf_t,af_t>;
using connection_t = nn2::Connection<nn2::EvoFloat<1,params>>;

using cppn_t = nn2::CPPN<neuron_t,connection_t,params>;

}//sq_cppn

class SQCPPNGenome: public Genome{
public:
    using cppn_t = sq_cppn::cppn_t;

    typedef std::shared_ptr<SQCPPNGenome> Ptr;
    typedef std::shared_ptr<const SQCPPNGenome> ConstPtr;
    SQCPPNGenome() : Genome(){
        cppn = cppn_t(sq_cppn::params::cppn::nb_inputs,sq_cppn::params::cppn::nb_outputs);
        type = "sq_cppn_genome";
    }
    SQCPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
        cppn = cppn_t(sq_cppn::params::cppn::nb_inputs,sq_cppn::params::cppn::nb_outputs);
        type = "sq_cppn_genome";
    }

    SQCPPNGenome(const cppn_t &cppn_gen,const sq_t &quadric_gen) :
    cppn(cppn_gen), quadric(quadric_gen){}

    SQCPPNGenome(const SQCPPNGenome &gen) :
        Genome(gen), cppn(gen.cppn),
        quadric(gen.quadric),
        feat_desc(gen.feat_desc),
        organ_position_desc(gen.organ_position_desc),
        matrix_desc(gen.matrix_desc){}
    ~SQCPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<SQCPPNGenome>(*this);
    }

    void init() override {
        cppn.init();
    }


    void random() override{
        cppn.random();
        quadric.random(randomNum);
    }

    void mutate() override {
        cppn.mutate();
        quadric.mutate(randomNum);
    }

    void crossover(const Genome::Ptr &partner,Genome::Ptr child) override {
        cppn_t cppn_child;
        cppn_t cppn_partner = std::dynamic_pointer_cast<SQCPPNGenome>(partner)->get_cppn();
        cppn.crossover(cppn_partner,cppn_child);
        
        sq_t quadric_child;
        sq_t quadric_partner = std::dynamic_pointer_cast<SQCPPNGenome>(partner)->get_quadric();
        quadric.crossover(quadric_partner,quadric_child,randomNum);
        *std::dynamic_pointer_cast<SQCPPNGenome>(child) = SQCPPNGenome(cppn_child,quadric_child);
        child->set_parameters(parameters);
        child->set_randNum(randomNum);
        std::dynamic_pointer_cast<SQCPPNGenome>(child)->set_parents_ids({_id,partner->id()});
    }

    void symmetrical_crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override{
        cppn_t cppn_child1, cppn_child2;
        cppn_t cppn_partner = std::dynamic_pointer_cast<SQCPPNGenome>(partner)->get_cppn();
        cppn.crossover(cppn_partner,cppn_child1);
        cppn_partner.crossover(cppn,cppn_child2);
        
        sq_t quadric_child1, quadric_child2;
        sq_t quadric_partner = std::dynamic_pointer_cast<SQCPPNGenome>(partner)->get_quadric();
        quadric.crossover(quadric_partner,quadric_child1,randomNum);
        quadric_partner.crossover(quadric,quadric_child2,randomNum);

        *std::dynamic_pointer_cast<SQCPPNGenome>(child1) = SQCPPNGenome(cppn_child1,quadric_child1);
        *std::dynamic_pointer_cast<SQCPPNGenome>(child2) = SQCPPNGenome(cppn_child2,quadric_child2);
        child1->set_parameters(parameters);
        child2->set_parameters(parameters);
        child1->set_randNum(randomNum);
        child2->set_randNum(randomNum);
        std::dynamic_pointer_cast<SQCPPNGenome>(child1)->set_parents_ids({_id,partner->id()});
        std::dynamic_pointer_cast<SQCPPNGenome>(child2)->set_parents_ids({partner->id(),_id});
    }

    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<SQCPPNGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<SQCPPNGenome>();
        oarch << *this;
        return sstream.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & cppn;
        arch & quadric;
        arch & feat_desc;
        arch & organ_position_desc;
        arch & matrix_desc;
    }

    
    void set_cppn(const cppn_t &c){cppn = c;}
    const cppn_t& get_cppn() const {return cppn;}

    void set_quadric(const sq_t &q){quadric = q;}
    const sq_t &get_quadric() const{return quadric;}

    int get_nb_neurons(){return cppn.get_nb_neurons();}
    int get_nb_connections(){return cppn.get_nb_connections();}

    const sim::FeaturesDesc& get_feat_desc() const {return feat_desc;}
    void set_feature_desc(const sim::FeaturesDesc& md){feat_desc = md;}

    const sim::OrganMatrixDesc& get_organ_position_desc() const {return organ_position_desc;}
    void set_organ_position_desc(const sim::OrganMatrixDesc& opd){organ_position_desc = opd;}

    const sim::MatrixDesc& get_matrix_desc() const {return matrix_desc;}
    void set_matrix_desc(const sim::MatrixDesc& mat){matrix_desc = mat;}

private:
    cppn_t cppn;
    sq_t quadric;
    sim::FeaturesDesc feat_desc;
    sim::OrganMatrixDesc organ_position_desc; //deprecated
    sim::MatrixDesc matrix_desc;
};

namespace  sq_cppn_decoder {
    using namespace sim::organ;
    using cppn_t = sq_cppn::cppn_t;


    void decode(const sq_t &quadric,
                cppn_t &cppn,
                skeleton::type& skeleton,
                organ_list_t &organ_list,
                int &number_voxels);
    void generate_skeleton(const sq_t &quadric,
                           skeleton::type& skeleton);
    void generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations);
// void clustering(const skeleton::coord_t &surface_coord, skeleton::coord_t &site_locations);
    void generate_organ_list(cppn_t &cppn,
                             const pcl::PointCloud<pcl::PointNormal>::Ptr site_locations,
                             organ_list_t &organ_list);
    int cppn_to_organ_type(cppn_t &cppn,const std::vector<double> &input);
};

namespace sq_cppn{

class QuadricsLog : public Logging
{
public:
    QuadricsLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class SQCPPNGenomeLog : public Logging
{
public:
    SQCPPNGenomeLog() : Logging(true){} //Logging at the end of the generation
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

}//sq_cppn
}//are

#endif //SQ_CPPN_GENOME_HPP
