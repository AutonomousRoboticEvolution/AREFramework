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

    // void random(const misc::RandNum::Ptr &rn){
    //     int nbr_comp = rn->randInt(1,8);
    //     positions.resize(nbr_comp);
    //     types.resize(nbr_comp);
    //     for(int n = 0; n < nbr_comp; n++){
    //         int rand_pos = 0;
    //         bool is_new_idx = true;
    //         do{
    //             is_new_idx = true;
    //             rand_pos = rn->randInt(0,100);
    //             for(int i = 0; i < n; i++){
    //                 if(rand_pos == positions[i]){
    //                     is_new_idx = false;
    //                     break;
    //                 }
    //             }
    //         }while(!is_new_idx);

    //         positions[n] = rand_pos;
    //         types[n] = rn->randInt(1,4);
    //     }
    // }

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

struct comp_genome_params{
    static double _position_mutation_rate;
    static double _type_mutation_rate;
    static double _modify_comp_list_mutation_rate;
    static double _add_remove_comp_prob;
};

template<typename comp_mut_params, typename quadrics_mut_params>
class SQGenome: public Genome{
public:
    using sq_t = quadric_t<quadrics_mut_params>;
    using cg_t = ComponentsGenome<comp_mut_params>;

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
        nbr_organs(gen.nbr_organs),
        feat_desc(gen.feat_desc),
        organ_position_desc(gen.organ_position_desc),
        matrix_desc(gen.matrix_desc){}
    ~SQGenome() override {}

    Genome::Ptr clone() const override {
        return std::make_shared<SQGenome>(*this);
    }

    void init() override {
    }


    void random() override{
        components_genome.random(randomNum);
        quadric.random(randomNum);
        nbr_organs = randomNum->randInt(4,8);
    }

    void mutate() override {
        components_genome.mutate(randomNum);
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
        cg_t cg_child;
        cg_t cg_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_components_genome();
        components_genome.crossover(cg_partner,cg_child,randomNum);
        
        sq_t quadric_child;
        sq_t quadric_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_quadric();
        quadric.crossover(quadric_partner,quadric_child,randomNum);
        *std::dynamic_pointer_cast<SQGenome>(child) = SQGenome(cg_child,quadric_child);
        child->set_parameters(parameters);
        child->set_randNum(randomNum);
        std::dynamic_pointer_cast<SQGenome>(child)->set_parents_ids({_id,partner->id()});
    }

    void symmetrical_crossover(const Genome::Ptr &partner,Genome::Ptr child1,Genome::Ptr child2) override{
        cg_t cg_child1, cg_child2;
        cg_t cg_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_components_genome();
        components_genome.crossover(cg_partner,cg_child1,randomNum);
        cg_partner.crossover(components_genome,cg_child2,randomNum);
        
        sq_t quadric_child1, quadric_child2;
        sq_t quadric_partner = std::dynamic_pointer_cast<SQGenome>(partner)->get_quadric();
        quadric.crossover(quadric_partner,quadric_child1,randomNum);
        quadric_partner.crossover(quadric,quadric_child2,randomNum);

        *std::dynamic_pointer_cast<SQGenome>(child1) = SQGenome(cg_child1,quadric_child1);
        *std::dynamic_pointer_cast<SQGenome>(child2) = SQGenome(cg_child2,quadric_child2);
        child1->set_parameters(parameters);
        child2->set_parameters(parameters);
        child1->set_randNum(randomNum);
        child2->set_randNum(randomNum);
        std::dynamic_pointer_cast<SQGenome>(child1)->set_parents_ids({_id,partner->id()});
        std::dynamic_pointer_cast<SQGenome>(child2)->set_parents_ids({partner->id(),_id});
    }

    void from_string(const std::string & str) override
    {
        std::stringstream sstream;
        sstream << str;
        boost::archive::text_iarchive iarch(sstream);
        iarch.register_type<SQGenome>();
        iarch >> *this;
    }
    std::string to_string() const override
    {
        std::stringstream sstream;
        boost::archive::text_oarchive oarch(sstream);
        oarch.register_type<SQGenome>();
        oarch << *this;
        return sstream.str();
    }

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & boost::serialization::base_object<Genome>(*this);
        arch & components_genome;
        arch & quadric;
        arch & nbr_organs;
        arch & feat_desc;
        arch & organ_position_desc;
        arch & matrix_desc;
    }

    void set_components_genome(const cg_t &cg){components_genome = cg;}
    const cg_t &get_components_genome(){return components_genome;}

    void set_quadric(const sq_t &q){quadric = q;}
    const sq_t &get_quadric() const{return quadric;}

    void set_nbr_organs(int nbr_o){nbr_organs = nbr_o;}
    int get_nbr_organs(){return nbr_organs;}


    const sim::FeaturesDesc& get_feat_desc() const {return feat_desc;}
    void set_feature_desc(const sim::FeaturesDesc& md){feat_desc = md;}

    const sim::OrganMatrixDesc& get_organ_position_desc() const {return organ_position_desc;}
    void set_organ_position_desc(const sim::OrganMatrixDesc& opd){organ_position_desc = opd;}

    const sim::MatrixDesc& get_matrix_desc() const {return matrix_desc;}
    void set_matrix_desc(const sim::MatrixDesc& mat){matrix_desc = mat;}

private:
    cg_t components_genome;
    sq_t quadric; //genome for the skeleton

    int nbr_organs = 0;
    sim::FeaturesDesc feat_desc;
    sim::OrganMatrixDesc organ_position_desc; //deprecated
    sim::MatrixDesc matrix_desc;
};


namespace  sq_decoder {
using namespace sim::organ;

template<typename sq_t>
void generate_skeleton(const sq_t &quadric,
                       skeleton::type& skeleton){
    PolyVox::Region region = skeleton.getEnclosingRegion();

    for(int32_t z = region.getLowerZ()+1; z < region.getUpperZ(); z += 1) {
        for(int32_t y = region.getLowerY()+1; y < region.getUpperY(); y += 1) {
            for(int32_t x = region.getLowerX()+1; x < region.getUpperX(); x += 1) {
                double ix = static_cast<double>(x)/static_cast<double>(region.getUpperX());
                double iy = static_cast<double>(y)/static_cast<double>(region.getUpperY());
                double iz = static_cast<double>(z)/static_cast<double>(region.getUpperZ());
                double output = quadric(ix,iy,iz);
                uint8_t voxel;

                if(output <= 1)
                    voxel = morph_const::filled_voxel;
                else
                    voxel = morph_const::empty_voxel;
                skeleton.setVoxel(x,y,z,voxel);
            }
        }
    }
}
template<typename cg_t>
void generate_organ_list(cg_t &comp_gen,
                         const pcl::PointCloud<pcl::PointNormal>::Ptr & site_locations,
                         int nbr_organs,
                         organ_list_t &organ_list){
    float number_sites = static_cast<float>(site_locations->size());
    float ratio = number_sites/100;
    std::vector<float> position(3);

    for(int i = 0; i < comp_gen.positions.size(); i++){
        int organ_type = comp_gen.types[i];
        int site_index = static_cast<int>(std::trunc(static_cast<float>(comp_gen.positions[i])*ratio));
        assert(site_index < site_locations->size());
        position[0] = site_locations->at(site_index).x*morph_const::voxel_real_size;
        position[1] = site_locations->at(site_index).y*morph_const::voxel_real_size;
        position[2] = site_locations->at(site_index).z*morph_const::voxel_real_size;
        position[2] += morph_const::matrix_size/2 * morph_const::voxel_real_size;
        std::vector<float> orientation(3);
        generate_orientation(site_locations->at(site_index).normal_x,
                             site_locations->at(site_index).normal_y,
                             site_locations->at(site_index).normal_z,orientation);
        organ_info organ(organ_type, position, orientation);
        organ_list.push_back(organ);
    }
}

void generate_organs_sites(skeleton::type &skeleton, pcl::PointCloud<pcl::PointNormal>::Ptr &sites_locations);

template<typename sq_t, typename cg_t>
void decode(const sq_t &quadric,
            cg_t &comp_gen,
            int nbr_organs,
            skeleton::type& skeleton,
            organ_list_t &organ_list,
            int &number_voxels){
    generate_skeleton<sq_t>(quadric,skeleton);
    skeleton::create_base(skeleton);
    skeleton::empty_space_for_head(skeleton);
    skeleton::remove_skeleton_regions(skeleton);
    skeleton::count_number_voxels(skeleton,number_voxels);
    pcl::PointCloud<pcl::PointNormal>::Ptr sites_locations = std::make_shared<pcl::PointCloud<pcl::PointNormal>>();
    generate_organs_sites(skeleton,sites_locations);
    generate_organ_list<cg_t>(comp_gen,sites_locations,nbr_organs,organ_list);
    sites_locations.reset();
}
}//sq_decoder

namespace sq_genome{
using sq_genome_t = SQGenome<comp_genome_params,quadric_params>;
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


}//sq_cppn

} //are


#endif //SQ_GENOME_HPP
