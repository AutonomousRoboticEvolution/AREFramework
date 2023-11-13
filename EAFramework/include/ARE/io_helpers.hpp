#ifndef IO_HELPERS_HPP
#define IO_HELPERS_HPP

#include <iostream>
#include <functional>
#include <boost/filesystem.hpp>
//#include <filesystem> //if c++17
#include "ARE/Settings.h"
#include "ARE/misc/utilities.h"
#include "ARE/Individual.h"

//namesapce fs = std::filesystem //if c++17
namespace fs = boost::filesystem;

namespace are {
namespace ioh{
/**
 * @brief Map of information about a particular morphological genome with the name of the info as key
 */
typedef settings::ParametersMap MorphGenomeInfo;

/**
 * @brief Map of MorphGenomeInfo with integer unique id key
 */
typedef std::map<int,settings::ParametersMap> MorphGenomeInfoMap;

/**
 * @brief move_file
 * @param origin
 * @param dest
 * @return
 */
bool move_file(const std::string& origin, const std::string& dest);

/**
 * @brief copy_file
 * @param origin
 * @param dest
 * @return
 */
bool copy_file(const std::string& origin, const std::string& dest);

/**
 * @brief load the information of all the morpholigical genomes from
 * @param folder
 * @param morph_gen_info
 */
void load_morph_genomes_info(const std::string &folder, MorphGenomeInfoMap& morph_gen_info);

template<class genome_t>
/**
 * @brief load the morphological genomes meeting the condition
 * @param folder
 * @param morph_gen_info
 * @param list_to_load
 * @param genomes
 */
void load_morph_genomes(const std::string &folder,
                        const std::vector<int> &list_to_load, std::map<int,are::Genome::Ptr>& genomes){

    std::string filepath, filename;
    std::vector<std::string> split_str;
    for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
        filepath = dirit.path().string();
        misc::split_line(filepath,"/",split_str);
        filename = split_str.back();
        misc::split_line(filename,"_",split_str);
        if(split_str[0] != "morph")
            continue;
        if(split_str[1] != "genome")
            continue;

        split_str.clear();
        misc::split_line(filename,"_",split_str);
        int id = std::stoi(split_str.back());
        bool load = [&]() -> bool{
            if(list_to_load[0] == -1)
                return true;
            for(const int& i : list_to_load)
                if(id == i)
                    return true;
            return false;
        }();
        if(!load)
            continue;

        std::shared_ptr<genome_t> genome = std::make_shared<genome_t>();
        genome->from_file(filepath);
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
}//ioh
}//are

#endif //IO_HELPERS_HPP
