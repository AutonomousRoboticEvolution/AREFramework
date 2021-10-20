#include "physicalER/io_helpers.hpp"

using namespace are;
namespace  st = settings;
namespace fs = boost::filesystem;

void phy::load_morph_genomes_info(const std::string &folder, MorphGenomeInfoMap &morph_gen_info){
    std::string genome_info_file(folder + std::string("/genomes_pool/morph_genomes_info.yml"));
    std::ifstream ifs(genome_info_file);
    if(!ifs){
        std::cerr << "unable to open " << genome_info_file << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> split_str;
    int id = 0;
    MorphGenomeInfo mgi;
    while(std::getline(ifs,line)){
        misc::split_line(line,",",split_str);

        if(split_str.empty())
            continue;
        if(split_str.size() == 1){

            misc::split_line(line,"_",split_str);
            id = std::stoi(split_str.back());
            morph_gen_info.emplace(id,mgi);
            mgi.clear();
            continue;
        }
        if(split_str.size() != 3)
            continue;
        st::Type::Ptr val = st::buildType(split_str[1]);
        val->fromString(split_str[2]);
        mgi.emplace(split_str[0],val);
    }
}

void phy::load_ids_to_be_evaluated(const std::string &folder,std::vector<int> &robot_ids){

    std::string eval_queue_folder(folder + std::string("/waiting_to_be_evaluated"));

    // loop through all list_of_organs files, and if a corresponding controller genome file does not exist, generate a random one
    std::string filepath, filename;
    std::vector<std::string> split_str;
    for(const auto &dirit : fs::directory_iterator(fs::path(eval_queue_folder))){
        filepath = dirit.path().string();
        misc::split_line(filepath,"/",split_str);
        filename = split_str.back();
        misc::split_line(filename,"_",split_str);
        if(split_str[0]+"_"+split_str[1]+"_"+split_str[2] == "list_of_organs"){
            // get the robot ID
            std::string robotID = split_str.back().substr(0,split_str.back().find("."));//filename.substr(15,filename.length()-19);
            robot_ids.push_back(std::stoi(robotID));
        }
    }
}

void phy::load_list_of_organs(const std::string &folder, const int& id, std::string &pi_address, std::string &list_of_organs){

    std::stringstream filepath;
    filepath << folder << "/waiting_to_be_evaluated/list_of_organs_" << id << ".csv";
    std::ifstream ifs(filepath.str());
    if(!ifs){
        std::cerr << "Could not open organs list file, was expecting it to be at: " << filepath.str() << std::endl;
        return;
    }

    std::string firstLine;
    std::getline(ifs,firstLine); // get first line
    pi_address = firstLine.substr(firstLine.find(",")+1,firstLine.find("\n")-2) ; // the first line should be "0,[pi address]\n"
    //if (verbose) std::cout<< "pi IP address: "<< pi_address << std::endl;
    for (std::string line; std::getline(ifs, line); )
        list_of_organs.append(line+"\n");
}

int phy::choice_of_robot_to_evaluate(const std::vector<int> &ids)
{
    std::cout << "Robots available for evaluation: " << std::endl;
    std::function<void(void)> print_ids = [&](){
        for(const int &id: ids){
            std::cout << id << std::endl;
        }
    };
    print_ids();

    bool good = false;
    int chosen_id;
    while(!good){
        good = true;
        std::cout << "Please enter the id of the robot to evaluate." << std::endl;
        std::string entry;
        std::cin >> entry;
        //check if the entry is a number. More precisely if the entry is a string convertible into an integer.
        try{
            chosen_id = std::stoi(entry);
        }catch(...){
            std::cerr << "You entry is not a number" << std::endl;
            good = false;
            print_ids();
            continue;
        }
        //If the entry is a number, check if it corresponds to an id in the list of ids.
        good = [&]() -> bool{
                for(const int& id: ids)
                    if(id == chosen_id)
                        return true;
                return false;}();
        if(!good){
            std::cerr << "The robot corresponding to the chosen id is not available for evaluation" << std::endl;
            print_ids();
        }
    }

    return chosen_id;
}
