#ifndef SQ_GENOME_HPP
#define SQ_GENOME_HPP

// #define EIGEN_MALLOC_ALREADY_ALIGNED true
// #define EIGEN_DEFAULT_ALIGN_BYTES 0

#include <boost/serialization/export.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "ARE/misc/RandNum.h"
#include "ARE/Genome.h"
#include "ARE/quadrics.hpp"
#include "ARE/Logging.h"
#include "simulatedER/skeleton_generation.hpp"
#include "simulatedER/Organ.h"
#include "simulatedER/morphology_descriptors.hpp"


#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

namespace are
{

template <typename Params>
struct ComponentsGenome{
    ComponentsGenome(){}
    ComponentsGenome(const ComponentsGenome &cg)
        : positions(cg.positions),
        types(cg.types){}

    std::vector<int> positions; //position of the components as an index of the list of the location sites.
    std::vector<int> types; //type of the components

    void random(const misc::RandNum::Ptr &rn){
        int nbr_comp = rn->randInt(1,8);
        positions.resize(nbr_comp);
        types.resize(nbr_comp);
        int lower = 0;
        int upper = std::round(100/nbr_comp);
        for(int n = 0; n < nbr_comp; n++){
            positions[n] = rn->randInt(lower,upper-1);
            lower+=std::round(100/nbr_comp);
            upper+=std::round(100/nbr_comp);
            types[n] = rn->randInt(1,4);
        }
    }

    void mutate(const misc::RandNum::Ptr &rn){
        //mutate the positions of the organ by incrementing and decrementing the position index
        for(int &p : positions){
            if(Params::_position_mutation_rate > rn->randFloat(0,1)){
                int flip = rn->randInt(0,1);
                if(flip == 0)
                    p = p - rn->randInt(1,5);
                else p = p + rn->randInt(1,5);
                if(p < 0)
                    p = 0;
                if(p>99)
                    p = 99;
            }
        }
        //mutate the type by randomly picking another type
        for(int &t : types){
            std::vector<int> type_list;
            for(int i = 1; i <= 4; i++){
                if(i != t)
                    type_list.push_back(i);
            }
            if(Params::_type_mutation_rate > rn->randFloat(0,1))
                t = type_list[rn->randInt(0,type_list.size()-1)];
        }
        if(Params::_modify_comp_list_mutation_rate > rn->randFloat(0,1)){
            if(Params::_add_remove_comp_prob > rn->randFloat(0,1)){//if random number lower then add or remove components probability then remove a component
                int idx = rn->randInt(0,types.size()-1);//pick a component randomly in the list
                types.erase(types.begin()+idx);
                positions.erase(positions.begin()+idx);
            }else if(types.size() < 8){//otherwise add a random component if there is not already 8 components
                types.push_back(rn->randInt(1,4));
                // int rand_pos = 0;
                // bool is_new_idx = true;
                // do{
                //     is_new_idx = true;
                //     rand_pos = rn->randInt(0,100);
                //     for(int i = 0; i < positions.size(); i++){
                //         if(rand_pos == positions[i]){
                //             is_new_idx = false;
                //             break;
                //         }
                //     }
                // }while(!is_new_idx);
                int rand_pos = rn->randInt(0,99);
                positions.push_back(rand_pos);
            }

        }

    }
    void crossover(const ComponentsGenome &partner, ComponentsGenome &child,const misc::RandNum::Ptr &rn){
        // No CROSSOVER for the moment
        child = partner;
    }

    void remove(size_t idx){
        types.erase(types.begin() + idx);
        positions.erase(positions.begin() + idx);
        types.shrink_to_fit();
        positions.shrink_to_fit();
    }

    std::string to_string() const{
        std::stringstream sstr;
        sstr << positions.size();
        for(const int &p : positions)
            sstr << ";" << p;
        for(const int &t : types)
            sstr << ";" << t;
        return sstr.str();
    }

    void from_string(const std::string &str){
        std::vector<std::string> splitted_str;
        misc::split_line(str,";",splitted_str);
        int nbr_comp = std::stoi(splitted_str[0]);
        for(int i = 1; i < nbr_comp+1; i++)
            positions.push_back(std::stoi(splitted_str[i]));
        for(int i = 1+nbr_comp; i < splitted_str.size();i++)
            types.push_back(std::stoi(splitted_str[i]));
    }

    template <class archive>
    void serialize(archive &arch, const unsigned int version)
    {
        arch & positions;
        arch & types;
    }
};

struct comp_mut_params{
    static double _position_mutation_rate;
    static double _type_mutation_rate;
    static double _modify_comp_list_mutation_rate;
    static double _add_remove_comp_prob;
};

using cg_t =  ComponentsGenome<comp_mut_params>;

class SQGenome: public Genome{
public:

    typedef std::shared_ptr<SQGenome> Ptr;
    typedef std::shared_ptr<const SQGenome> ConstPtr;
    SQGenome() : Genome(){
        type = "sq_genome";
    }
    SQGenome(const misc::RandNum::Ptr &rn, const settings::ParametersMapPtr &param) :
        Genome(rn,param){
        type = "sq_genome";
    }

    SQGenome(const cg_t &comp_gen,const sq_t &quadric_gen) :
    components_genome(comp_gen), quadric(quadric_gen){}

    SQGenome(const SQGenome &gen) :
        Genome(gen), 
        components_genome(gen.components_genome),
        quadric(gen.quadric),
        feat_desc(gen.feat_desc),
        organ_position_desc(gen.organ_position_desc),
        matrix_desc(gen.matrix_desc){}
    ~SQGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<SQGenome>(*this);
    }

    void init() override {
    }


    void random() override;

    void mutate() override;

    void crossover(const Genome::Ptr &partner,Genome::Ptr child) override;

    void symmetrical_crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override;

    void from_string(const std::string & str) override;

    std::string to_string() const override;

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & components_genome;
        arch & quadric;
        arch & feat_desc;
        arch & organ_position_desc;
        arch & matrix_desc;
    }

    void set_components_genome(const cg_t &cg){components_genome = cg;}
    const cg_t &get_components_genome(){return components_genome;}

    void set_quadric(const sq_t &q){quadric = q;}
    const sq_t &get_quadric() const{return quadric;}


    const sim::FeaturesDesc& get_feat_desc() const {return feat_desc;}
    void set_feature_desc(const sim::FeaturesDesc& md){feat_desc = md;}

    const sim::OrganMatrixDesc& get_organ_position_desc() const {return organ_position_desc;}
    void set_organ_position_desc(const sim::OrganMatrixDesc& opd){organ_position_desc = opd;}

    const sim::MatrixDesc& get_matrix_desc() const {return matrix_desc;}
    void set_matrix_desc(const sim::MatrixDesc& mat){matrix_desc = mat;}

private:
    cg_t components_genome;
    sq_t quadric; //genome for the skeleton

    sim::FeaturesDesc feat_desc;
    sim::OrganMatrixDesc organ_position_desc; //deprecated
    sim::MatrixDesc matrix_desc;
};


namespace  sq_decoder {
using namespace sim::organ;

void decode(const sq_t &quadric,
            cg_t &comp_gen,
            skeleton::type& skeleton,
            organ_list_t &organ_list,
            int &number_voxels);

void generate_skeleton(const sq_t &quadric,
                       skeleton::type& skeleton);

void generate_organ_list(cg_t &comp_gen,
                         const pcl::PointCloud<pcl::PointNormal>::Ptr site_locations,
                         organ_list_t &organ_list);

void generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations);


}//sq_decoder

namespace sq_genome{
class QuadricsLog : public Logging
{
public:
    QuadricsLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class CompGenLog: public Logging
{
public:
    CompGenLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};

class SQGenomeLog : public Logging
{
public:
    SQGenomeLog() : Logging(true){} //Logging at the end of the generation
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
}//sq_genome

} //are


#endif //SQ_GENOME_HPP
