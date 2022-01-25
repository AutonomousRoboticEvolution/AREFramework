#ifndef IO_HELPERS_HPP
#define IO_HELPERS_HPP

#include <iostream>
#include <functional>
#include <boost/filesystem.hpp>
//#include <filesystem> //if c++17
#include "ARE/Settings.h"
#include "ARE/misc/utilities.h"
#include "ARE/Individual.h"
#include <simLib.h>

//namesapce fs = std::filesystem //if c++17
namespace fs = boost::filesystem;

namespace are {
namespace phy {

/**
 * @brief Map of information about a particular morphological genome with the name of the info as key
 */
typedef std::map<std::string, are::settings::Type::ConstPtr> MorphGenomeInfo;

/**
 * @brief Map of MorphGenomeInfo with integer unique id key
 */
typedef std::map<int,std::map<std::string, are::settings::Type::ConstPtr>> MorphGenomeInfoMap;


/**
 * @brief load the information of all the morpholigical genomes from
 * @param folder
 * @param morph_gen_info
 */
void load_morph_genomes_info(const std::string &folder, MorphGenomeInfoMap& morph_gen_info);

template<class Genome>
/**
 * @brief load the morphological genomes meeting the condition
 * @param folder
 * @param morph_gen_info
 * @param list_to_load
 * @param genomes
 */
void load_morph_genomes(const std::string &folder,
                        const std::vector<int> list_to_load, std::map<int,Genome>& genomes){

    std::string filepath, filename;
    std::list<std::string> split_str;
    for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
        filepath = dirit.path().string();
        misc::split_line(filepath,"/",split_str);
        filename = split_str.back();
        if(filename.substr(0,filename.find("_")) != "morph")
            continue;

        split_str.clear();
        misc::split_line(filename,"_",split_str);
        int id = std::stoi(split_str.back());
        bool load = [&]() -> bool{
            for(const int& i : list_to_load)
                if(id == i)
                    return true;
            return false;
        }();
        if(!load)
            continue;

        Genome genome;
        genome.from_string(filepath);
        genomes.emplace(id,genome);
    }

}
/**
 * @brief load the ids of the robot needed to be evaluated
 * @param folder of the database of the current experiement
 * @param list of robot ids (return)
 */
void load_ids_to_be_evaluated(const std::string &folder,std::vector<int> &robot_ids);

/**
 * @brief load_list_of_organs
 * @param folder
 * @param ip_address
 * @param list_of_organs
 */
void load_list_of_organs(const std::string &folder, const int &id, std::string &ip_address, std::string &list_of_organs);

void load_controller_genome(const std::string &folder, const int &id, const Genome::Ptr& genome);

/**
 * @brief load the number of each organs type of a particular list_of_organs file
 * @param folder of the current experiment
 * @param id of the robot
 * @param wheels
 * @param joints
 * @param sensors
 */
void load_nbr_organs(const std::string &folder, const int& id, int &wheels, int& joints, int& sensors);


/**
 * @brief choice_of_robot_to_evaluate
 * @param ids
 * @return
 */
int choice_of_robot_to_evaluate(const std::vector<int> &ids);

template<class Ind>
/**
 * @brief write in the waiting_to_be_built folder all the blueprints of a given population
 * @param folder of the database
 * @param population
 */
void write_morph_blueprints(const std::string& folder,const std::vector<Individual::Ptr> &population){
    int id = 0;
    for(const auto& ind: population){
        id = ind->get_morph_genome()->id();

        // Export blueprint
        std::stringstream sst_blueprint;
        sst_blueprint << "blueprint_" << id << ".csv";
        std::ofstream ofs(folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str());
        if(!ofs)
        {
            std::cerr << "unable to open : " << folder + std::string("/waiting_to_be_built/")  + sst_blueprint.str() << std::endl;
            return;
        }
        std::vector<int> tempOrganTypes = std::dynamic_pointer_cast<Ind>(ind)->getListOrganTypes();
        std::vector<std::vector<float>> tempOrganPos = std::dynamic_pointer_cast<Ind>(ind)->getListOrganPos();
        std::vector<std::vector<float>> tempOrganOri = std::dynamic_pointer_cast<Ind>(ind)->getListOrganOri();
        for (int i = 0; i < tempOrganTypes.size(); i++) {
            ofs << "0" << "," << tempOrganTypes.at(i) << ","
                          << tempOrganPos.at(i).at(0) << "," << tempOrganPos.at(i).at(1) << ","
                          << tempOrganPos.at(i).at(2) << ","
                          << tempOrganOri.at(i).at(0) << "," << tempOrganOri.at(i).at(1) << ","
                          << tempOrganOri.at(i).at(2) << ","
                          << std::endl;
        }
        ofs.close();
    }
}

template<class Ind>
/**
 * @brief write in the waiting_to_be_built folder  all the skeleton meshes of a given population
 * @param folder
 * @param population
 */
void write_morph_meshes(const std::string& folder, const std::vector<Individual::Ptr> &population){
    int id = 0;
    for(const auto& ind: population){
        id = ind->get_morph_genome()->id();

        const auto **verticesMesh = new const simFloat *[2];
        const auto **indicesMesh = new const simInt *[2];
        auto *verticesSizesMesh = new simInt[2];
        auto *indicesSizesMesh = new simInt[2];
        verticesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListVertices().data();
        verticesSizesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListVertices().size();
        indicesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListIndices().data();
        indicesSizesMesh[0] = std::dynamic_pointer_cast<Ind>(ind)->getSkeletonListIndices().size();

        std::stringstream filepath;
        filepath << folder << "/waiting_to_be_built/mesh_" << id << ".stl";

        //fileformat: the fileformat to export to:
        //  0: OBJ format, 3: TEXT STL format, 4: BINARY STL format, 5: COLLADA format, 6: TEXT PLY format, 7: BINARY PLY format
        simExportMesh(3, filepath.str().c_str(), 0, 1.0f, 1, verticesMesh, verticesSizesMesh, indicesMesh, indicesSizesMesh, nullptr, nullptr);

        delete[] verticesMesh;
        delete[] verticesSizesMesh;
        delete[] indicesMesh;
        delete[] indicesSizesMesh;
    }
}

/**
 * @brief write in the waiting_to_be_built folder  all the morph genomes of a given population
 * @param folder
 * @param population
 */
void write_morph_genomes(const std::string &folder, const std::vector<Individual::Ptr> &population);

/**
 * @brief Adding a morph genome to the genome pool which consist in two actions:
 *  - move a morph genome from the waiting_to_be_built folder to the genomes pool folder
 *  - add entry in the morph genome info file
 * @param folder of the current experiment
 * @param id of the robot
 */
void add_morph_genome_to_gp(const std::string &folder,int id, const MorphGenomeInfo &morph_info);

}//phy
}//are

#endif //IO_HELPERS_HPP
