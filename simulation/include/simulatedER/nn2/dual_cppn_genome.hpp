#pragma once

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <pcl/features/normal_3d.h>
// #include <pcl/visualization/pcl_visualizer.h>
#include <pcl/filters/voxel_grid.h>

#include "nn2/cppn.hpp"
#include "ARE/Genome.h"
#include "ARE/Logging.h"
#include "simulatedER/morphology_descriptors.hpp"
#include "simulatedER/morphology_constants.hpp"
#include "simulatedER/skeleton_generation.hpp"

namespace are {

namespace dual_cppn{
namespace skeleton{
struct params{
    struct cppn{
        static constexpr int _mutation_type = 0;
        static constexpr bool _mutate_connections = true;
        static constexpr bool _mutate_neurons = true;
        static constexpr float _mutation_rate = 0.5f;
        static constexpr float _rate_mutate_conn = 0.1f;
        static constexpr float _rate_mutate_neur = 0.1f;
        static constexpr float _rate_add_neuron = 0.1f;
        static constexpr float _rate_del_neuron = 0.1f;
        static constexpr float _rate_add_conn = 0.1f;
        static constexpr float _rate_del_conn = 0.1f;
        static constexpr float _rate_change_conn = 0.1f;
        static constexpr float _rate_crossover = 0.5f;

        static constexpr size_t _min_nb_neurons = 0;
        static constexpr size_t _max_nb_neurons = 5;
        static constexpr size_t _min_nb_conns = 10;
        static constexpr size_t _max_nb_conns = 100;

        static constexpr int nb_inputs = 3;
        static constexpr int nb_outputs = 1;
        static constexpr double _expressiveness = 1.f;
    };
    struct evo_float{
        static float constexpr mutation_rate = 0.1f;
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
}//skeleton

namespace organs{
struct params{
    struct cppn{
        static constexpr int _mutation_type = 0;
        static constexpr bool _mutate_connections = true;
        static constexpr bool _mutate_neurons = true;
        static constexpr float _mutation_rate = 0.5f;
        static constexpr float _rate_mutate_conn = 0.1f;
        static constexpr float _rate_mutate_neur = 0.1f;
        static constexpr float _rate_add_neuron = 0.1f;
        static constexpr float _rate_del_neuron = 0.1f;
        static constexpr float _rate_add_conn = 0.1f;
        static constexpr float _rate_del_conn = 0.1f;
        static constexpr float _rate_change_conn = 0.1f;
        static constexpr float _rate_crossover = 0.5f;

        static constexpr size_t _min_nb_neurons = 0;
        static constexpr size_t _max_nb_neurons = 2;
        static constexpr size_t _min_nb_conns = 10;
        static constexpr size_t _max_nb_conns = 100;

        static constexpr int nb_inputs = 3;
        static constexpr int nb_outputs = 1;
        static constexpr double _expressiveness = 10.f;
    };
    struct evo_float{
        static constexpr float mutation_rate = 0.1f;
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
}//organs
}//dual_cppn

using skel_cppn_t = dual_cppn::skeleton::cppn_t;
using org_cppn_t = dual_cppn::organs::cppn_t;

class DualCPPNGenome : public Genome
{
public:
    typedef std::shared_ptr<DualCPPNGenome> Ptr;
    typedef std::shared_ptr<const DualCPPNGenome> ConstPtr;

    DualCPPNGenome() : Genome(){
        _skel_cppn = skel_cppn_t(dual_cppn::skeleton::params::cppn::nb_inputs,dual_cppn::skeleton::params::cppn::nb_outputs);
        _org_cppn = org_cppn_t(dual_cppn::organs::params::cppn::nb_inputs,dual_cppn::organs::params::cppn::nb_outputs);
        type = "dual_cppn_genome";
    }
    DualCPPNGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
        _skel_cppn = skel_cppn_t(dual_cppn::skeleton::params::cppn::nb_inputs,dual_cppn::skeleton::params::cppn::nb_outputs);
        _org_cppn = org_cppn_t(dual_cppn::organs::params::cppn::nb_inputs,dual_cppn::organs::params::cppn::nb_outputs);
        type = "dual_cppn_genome";

    }
    DualCPPNGenome(const skel_cppn_t &skel_cppn_gen, const org_cppn_t &org_cppn_gen) : 
        _skel_cppn(skel_cppn_gen), _org_cppn(org_cppn_gen){}

    DualCPPNGenome(const DualCPPNGenome &gen) :
        Genome(gen), _skel_cppn(gen._skel_cppn), _org_cppn(gen._org_cppn), feat_desc(gen.feat_desc), 
        organ_position_desc(gen.organ_position_desc), matrix_desc(gen.matrix_desc){}

    ~DualCPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<DualCPPNGenome>(*this);
    }

    void init() override {
        _skel_cppn.init();
        _org_cppn.init();
    }

    void fixed_structure(){
        _skel_cppn.build_fixed_structure();
        _org_cppn.build_fixed_structure();
    }

    void random() override{
        _skel_cppn.random();
        _org_cppn.random();
    }

    void mutate() override {
        _skel_cppn.mutate();
        _org_cppn.mutate();
    }

    void crossover(const Genome::Ptr &partner,Genome::Ptr child) override {
        skel_cppn_t skel_cppn_child;
        std::pair<skel_cppn_t,org_cppn_t> cppns_partner = std::dynamic_pointer_cast<DualCPPNGenome>(partner)->get_cppns();
        _skel_cppn.crossover(cppns_partner.first,skel_cppn_child);
        org_cppn_t org_cppn_child;
        _org_cppn.crossover(cppns_partner.second,org_cppn_child);
        *std::dynamic_pointer_cast<DualCPPNGenome>(child) = DualCPPNGenome(skel_cppn_child,org_cppn_child);
        child->set_parameters(parameters);
        child->set_randNum(randomNum);
        child->set_parents_ids({_id,partner->id()});
    }

    void symmetrical_crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override{
        skel_cppn_t skel_cppn_child1, skel_cppn_child2;
        std::pair<skel_cppn_t,org_cppn_t> cppns_partner = std::dynamic_pointer_cast<DualCPPNGenome>(partner)->get_cppns();
        _skel_cppn.crossover(cppns_partner.first,skel_cppn_child1);
        cppns_partner.first.crossover(_skel_cppn,skel_cppn_child2);
        org_cppn_t org_cppn_child1, org_cppn_child2;
        _org_cppn.crossover(cppns_partner.second,org_cppn_child1);
        cppns_partner.second.crossover(_org_cppn,org_cppn_child2);
        *std::dynamic_pointer_cast<DualCPPNGenome>(child1) = DualCPPNGenome(skel_cppn_child1,org_cppn_child1);
        *std::dynamic_pointer_cast<DualCPPNGenome>(child2) = DualCPPNGenome(skel_cppn_child2,org_cppn_child2);
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
        iarch.register_type<DualCPPNGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<DualCPPNGenome>();
        oarch << *this;
        return sstream.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & _skel_cppn;
        arch & _org_cppn;
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

    void set_cppns(const skel_cppn_t &sc, const org_cppn_t &oc){_skel_cppn = sc;_org_cppn = oc;}
    std::pair<skel_cppn_t,org_cppn_t> get_cppns() const {return std::make_pair(_skel_cppn,_org_cppn);}
    void set_matrix_4d(const std::vector<std::vector<double>> m4d){matrix_4d = m4d;}
    const std::vector<std::vector<double>>& get_matrix_4d(){return matrix_4d;}

    std::array<unsigned int,2> get_nb_neurons(){return {_skel_cppn.get_nb_neurons(),_org_cppn.get_nb_neurons()};}
    std::array<unsigned int,2> get_nb_connections(){return {_skel_cppn.get_nb_connections(),_org_cppn.get_nb_connections()};}

    int get_generation(){return generation;}
    void incr_generation(){generation++;}

    void set_randNum(const misc::RandNum::Ptr& rn) override{
        randomNum = rn;
        //nn2::rgen_t::gen.seed(randomNum->getSeed());
    }

private:
    skel_cppn_t _skel_cppn;
    org_cppn_t _org_cppn;
    sim::FeaturesDesc feat_desc;
    sim::OrganMatrixDesc organ_position_desc;
    sim::MatrixDesc matrix_desc;
    int generation=0;
    std::vector<std::vector<double>> matrix_4d;
};

namespace dual_cppn_decoder {

using namespace sim::organ;

void decode(skel_cppn_t &skel_cppn,
            org_cppn_t &org_cppn,
            skeleton::type& skeleton,
            int nbr_organs,
            organ_list_t &organ_list,
            int &number_voxels,
            bool growing = false);
void generate_skeleton(skel_cppn_t &cppn,
                       skeleton::type& skeleton);
void generate_skeleton_growing(skel_cppn_t &cppn,
                       skeleton::type& skeleton);
void generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations);
void generate_organ_list(org_cppn_t &cppn,
                         const pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations,
                         int nbr_organs,
                         organ_list_t &organ_list);
int cppn_to_organ_type(org_cppn_t &cppn,const std::vector<double> &input);
};

namespace dual_cppn {

class GenomeLog : public Logging
{
public:
    GenomeLog() : Logging(true){} //Logging at the end of the generation
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


