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
#include "simulatedER/skeleton_generation.hpp"
#include "simulatedER/Organ.h"
#include "simulatedER/morphology_descriptors.hpp"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace are{


struct quadric_param_t{
    quadric_param_t(double _a, double _b, double _c,
                    double _u, double _v,
                    double _r, double _s, double _t);
    quadric_param_t(const quadric_param_t& qp):a(qp.a), b(qp.b), c(qp.c),
        u(qp.u), v(qp.v), r(qp.r),s(qp.s),t(qp.t){}

    double a;
    double b;
    double c;
    double u;
    double v;
    double r;
    double s;
    double t;
    std::vector<double> get_params(){return {a,b,c,u,v,r,s,t};}
    void random(const misc::RandNum::Ptr &rn);
    std::string to_string() const;
    void from_string(const std::string& str);

    template <class archive>
    void serialize(archive &arch, const unsigned int version)
    {
        arch & a;
        arch & b;
        arch & c;
        arch & u;
        arch & v;
        arch & r;
        arch & s;
        arch & t;
    }
};

template <typename Params>
struct quadric_t{
    quadric_t(): p1(1,1,1,1,1,1,1,1), p2(1,1,1,1,1,1,1,1),
                 p3(1,1,1,1,1,1,1,1), p4(1,1,1,1,1,1,1,1){}
    quadric_t(const quadric_t &q):
        p1(q.p1),p2(q.p2),p3(q.p3),p4(q.p4),
        x_symmetry(q.x_symmetry),y_symmetry(q.y_symmetry){}

    quadric_param_t p1;
    quadric_param_t p2;
    quadric_param_t p3;
    quadric_param_t p4;
    int x_symmetry = 1;
    int y_symmetry = 1;

    void init_params(quadric_param_t &p, int shape_family = 0){
        if(shape_family == 0){ //superellipsoid
            p = quadric_param_t(1,1,1,1,1,1,1,1);
        }else if(shape_family == 1){//pyramid
            p = quadric_param_t(1,1,1,1,2,1,1,1);
        }else if(shape_family == 2){//pyramid 2
            p = quadric_param_t(1,1,1,2,1,1,1,1);
        }else if(shape_family == 3){//hyperpoloid 1-sheet
            p = quadric_param_t(1,1,1,1,1,-5,5,5);
        }else if(shape_family == 4){//diagonal wall
            p = quadric_param_t(1,1,1,1,2,-1,1,1);
        }else if(shape_family == 5){//diagonal wall 2
            p = quadric_param_t(1,1,1,2,1,-1,1,1);
        }else if(shape_family == 6){//hyperboloid 2-sheets
            p = quadric_param_t(1,1,1,1,2,1,-1,-1);
        }else if(shape_family == 7){//hyperboloid 2-sheets 2
            p = quadric_param_t(1,1,1,1,2,1,-1,1);
        }else if(shape_family == 8){//long pyramid
            p = quadric_param_t(1,1,1,2,1,1,-1,1);
        }
    }

    void random(const misc::RandNum::Ptr &rn){
        int initial_shape_family = rn->randInt(0,8);
        init_params(p1,initial_shape_family);

        x_symmetry = rn->randInt(0,1);
        y_symmetry = rn->randInt(0,1);
        //initial variation
        p1.random(rn);
        if(x_symmetry == 1 && y_symmetry == 1){
            p2 = p1;
            p3 = p1;
            p4 = p1;
        }
        else if(x_symmetry == 1 && y_symmetry == 0){
            initial_shape_family = rn->randInt(0,8);
            init_params(p2,initial_shape_family);
            p2.random(rn);
            p3 = p1;
            p4 = p2;
        }
        else if(x_symmetry == 0 && y_symmetry == 1){
            initial_shape_family = rn->randInt(0,8);
            init_params(p3,initial_shape_family);
            p3.random(rn);
            p2 = p1;
            p4 = p3;
        }
        else{
            initial_shape_family = rn->randInt(0,8);
            init_params(p2,initial_shape_family);
            p2.random(rn);
            initial_shape_family = rn->randInt(0,8);
            init_params(p3,initial_shape_family);
            p3.random(rn);
            initial_shape_family = rn->randInt(0,8);
            init_params(p4,initial_shape_family);
            p4.random(rn);
        }

    }
    void mutate(const misc::RandNum::Ptr &rn){
        std::vector<double> quad_param = p1.get_params();
        double params[4] = {Params::_mutation_rate,Params::_sigma,Params::_ubound,Params::_lbound};
        mutators::gaussian(quad_param,params,rn);
        p1 = quadric_param_t(quad_param[0],
                             quad_param[1],
                             quad_param[2],
                             quad_param[3],
                             quad_param[4],
                             quad_param[5],
                             quad_param[6],
                             quad_param[7]);
        if(x_symmetry == 1 && y_symmetry == 1){
            p2 = p1;
            p3 = p1;
            p4 = p1;
        }
        else if(x_symmetry == 1 && y_symmetry == 0){
            quad_param = p2.get_params();
            mutators::gaussian(quad_param,params,rn);
            p2 = quadric_param_t(quad_param[0],
                                 quad_param[1],
                                 quad_param[2],
                                 quad_param[3],
                                 quad_param[4],
                                 quad_param[5],
                                 quad_param[6],
                                 quad_param[7]);
            p3 = p1;
            p4 = p2;
        }
        else if(x_symmetry == 0 && y_symmetry == 1){
            quad_param = p3.get_params();
            mutators::gaussian(quad_param,params,rn);
            p3 = quadric_param_t(quad_param[0],
                                 quad_param[1],
                                 quad_param[2],
                                 quad_param[3],
                                 quad_param[4],
                                 quad_param[5],
                                 quad_param[6],
                                 quad_param[7]);
            p2 = p1;
            p4 = p3;
        }
        else{
            quad_param = p2.get_params();
            mutators::gaussian(quad_param,params,rn);
            p2 = quadric_param_t(quad_param[0],
                                 quad_param[1],
                                 quad_param[2],
                                 quad_param[3],
                                 quad_param[4],
                                 quad_param[5],
                                 quad_param[6],
                                 quad_param[7]);
            quad_param = p3.get_params();
            mutators::gaussian(quad_param,params,rn);
            p3 = quadric_param_t(quad_param[0],
                                 quad_param[1],
                                 quad_param[2],
                                 quad_param[3],
                                 quad_param[4],
                                 quad_param[5],
                                 quad_param[6],
                                 quad_param[7]);
            quad_param = p4.get_params();
            mutators::gaussian(quad_param,params,rn);
            p4 = quadric_param_t(quad_param[0],
                                 quad_param[1],
                                 quad_param[2],
                                 quad_param[3],
                                 quad_param[4],
                                 quad_param[5],
                                 quad_param[6],
                                 quad_param[7]);
        }


    }
    void crossover(const quadric_t &q1, quadric_t &q2,const misc::RandNum::Ptr &rn){
        // No CROSSOVER for the moment
        // std::vector<double> quad_param = {a,b,c,u,v,r,s,t};
        // std::vector<double> quad_param2 = {q1.a,q1.b,q1.c,q1.u,q1.v,q1.r,q1.s,q1.t};
        // std::vector<double> child1, child2;
        // double params[3] = {Params::_lbound,Params::_ubound,Params::_eta};
        // crossovers::sbx(quad_param,quad_param2,child1,child2,params,rn);
        // q2.a = child1[0];
        // q2.b = child1[1];
        // q2.c = child1[2];
        // q2.u = child1[3];
        // q2.v = child1[4];
        // q2.r = child1[5];
        // q2.s = child1[6];
        // q2.t = child1[7];
        q2 = q1;
    }
    double operator () (double x, double y, double z) const{
        auto quadric_fct = [](double X, double Y, double Z,quadric_param_t p) -> double{
            return pow(p.r*pow(X/p.a,2/p.u) + p.s*pow(Y/p.b,2/p.u),p.u/p.v) + p.t*pow(Z/p.c,2/p.v);
        };
        if(x_symmetry == 1 && y_symmetry == 1)
            return quadric_fct(fabs(x),fabs(y),z+1,p1);

        if(x >= 0 && y >= 0)
            return quadric_fct(x,y,z+1,p1);
        else if(x < 0 && y >= 0)
            return quadric_fct(fabs(x),y,z+1,p2);
        else if(x >= 0 && y < 0)
            return quadric_fct(x,fabs(y),z+1,p3);
        else
            return quadric_fct(fabs(x),fabs(y),z+1,p4);
    }

    std::string to_string() const {
        std::stringstream sstr;
        sstr << x_symmetry << ";" << y_symmetry << ";" << p1.to_string() << ";" << p2.to_string() << ";" << p3.to_string() << ";" << p4.to_string();
        return sstr.str();
    }

    void from_string(const std::string& str){
        std::vector<std::string> splitted_str;
        misc::split_line(str,";",splitted_str);
        x_symmetry = std::stoi(splitted_str[0]);
        y_symmetry = std::stoi(splitted_str[1]);
        p1.from_string(splitted_str[2]);
        p2.from_string(splitted_str[3]);
        p3.from_string(splitted_str[4]);
        p4.from_string(splitted_str[5]);
    }

    template <class archive>
    void serialize(archive &arch, const unsigned int version)
    {
        arch & x_symmetry;
        arch & y_symmetry;
        arch & p1;
        arch & p2;
        arch & p3;
        arch & p4;
    }
};

struct quadric_params{
    static constexpr double _mutation_rate = 0.1f;
    static constexpr double _sigma = 0.1f;
    static constexpr double _lbound = -2;
    static constexpr double _ubound = 2;
};

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
    using sq_t = quadric_t<quadric_params>;

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
        nbr_organs(gen.nbr_organs),
        feat_desc(gen.feat_desc),
        organ_position_desc(gen.organ_position_desc),
        matrix_desc(gen.matrix_desc){}
    ~SQCPPNGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<SQCPPNGenome>(*this);
    }

    void init() override {
        sq_cppn::params::cppn::_expressiveness = settings::getParameter<settings::Double>(parameters,"#cppnExpressiveness").value;
        cppn.init();
    }


    void random() override{
        sq_cppn::params::cppn::_expressiveness = settings::getParameter<settings::Double>(parameters,"#cppnExpressiveness").value;
        cppn.random();
        quadric.random(randomNum);
        nbr_organs = randomNum->randInt(4,8);
    }

    void mutate() override {
        cppn.mutate();
        quadric.mutate(randomNum);
        if(randomNum->randDouble(0,1) < quadric_params::_mutation_rate){
            nbr_organs += randomNum->randInt(-1,1);
            if(nbr_organs < 4)
                nbr_organs = 4;
            else if(nbr_organs > 8)
                nbr_organs = 8;
        }
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
        arch & nbr_organs;
        arch & feat_desc;
        arch & organ_position_desc;
        arch & matrix_desc;
    }

    
    void set_cppn(const cppn_t &c){cppn = c;}
    const cppn_t& get_cppn() const {return cppn;}

    void set_quadric(const sq_t &q){quadric = q;}
    const sq_t &get_quadric() const{return quadric;}

    void set_nbr_organs(int nbr_o){nbr_organs = nbr_o;}
    int get_nbr_organs(){return nbr_organs;}

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
    int nbr_organs = 0;
    sim::FeaturesDesc feat_desc;
    sim::OrganMatrixDesc organ_position_desc; //deprecated
    sim::MatrixDesc matrix_desc;
};

namespace  sq_cppn_decoder {
    using namespace sim::organ;
    using sq_t = quadric_t<quadric_params>;
    using cppn_t = sq_cppn::cppn_t;


    void decode(const sq_t &quadric,
                cppn_t &cppn,
                int nbr_organs,
                skeleton::type& skeleton,
                organ_list_t &organ_list,
                int &number_voxels);
    void generate_skeleton(const sq_t &quadric,
                           skeleton::type& skeleton);
    void generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations);
// void clustering(const skeleton::coord_t &surface_coord, skeleton::coord_t &site_locations);
    void generate_organ_list(cppn_t &cppn,
                             const pcl::PointCloud<pcl::PointNormal>::Ptr & site_locations,
                             int nbr_organs,
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
