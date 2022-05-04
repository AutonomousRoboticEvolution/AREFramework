#include "ARE/crossbreeding.hpp"

using namespace are;

Crossbreeding::selection_fct_t
Crossbreeding::selection_fct::best_fitness = [](int nbr, const ioh::MorphGenomeInfoMap& mgi) -> std::vector<int> {
    std::vector<std::pair<int,float>> fitnesses;
    for(const auto& elt: mgi)
        fitnesses.push_back(std::make_pair(elt.first,settings::getParameter<settings::Float>(elt.second,"fitness").value));

    std::sort(fitnesses.begin(),fitnesses.end(),[](const auto& a, const auto& b)->bool{return a.second > b.second;}); //sort in decreasing order
    std::vector<int> list_of_ids;
    for(int i = 0; i < nbr; i++)
        list_of_ids.push_back(fitnesses[i].first);
    return list_of_ids;
};

Crossbreeding::trigger_criterion_t
Crossbreeding::trigger_criterion::never = [](const ioh::MorphGenomeInfoMap&, const settings::ParametersMapPtr&) -> bool{
    return false;
};

Crossbreeding::trigger_criterion_t
Crossbreeding::trigger_criterion::always = [](const ioh::MorphGenomeInfoMap&, const settings::ParametersMapPtr&) -> bool{
    return true;
};

Crossbreeding::trigger_criterion_t
Crossbreeding::trigger_criterion::manual = [](const ioh::MorphGenomeInfoMap&, const settings::ParametersMapPtr&) -> bool{
    std::cout << "Activate Crossbreeding (Y|y|yes or N|n|no) default : N" << std::endl;
    std::string entry;
    std::cin >> entry;
    if(entry != "Y" && entry != "yes" && entry != "y" &&
            entry != "N" && entry != "no" && entry != "n")
        return false;
    return entry == "Y" || entry == "yes" || entry == "y";
};

Crossbreeding::trigger_criterion_t
Crossbreeding::trigger_criterion::low_fitness = [](const ioh::MorphGenomeInfoMap& mgi, const settings::ParametersMapPtr& param) -> bool{
    float threshold = settings::getParameter<settings::Float>(param,"#crossbreedingFitnessThreshold").value;
    float average = 0;
    for(const auto &elt: mgi)
        average += settings::cast<settings::Float>(elt.second.at("fitness"))->get_value();
    average /= static_cast<float>(mgi.size());
    return average <= threshold;
};

Crossbreeding::trigger_criterion_t
Crossbreeding::trigger_criterion::low_diversity = [](const ioh::MorphGenomeInfoMap& mgi, const settings::ParametersMapPtr& param) -> bool{
    float threshold = settings::getParameter<settings::Float>(param,"#crossbreedingDiversityThreshold").value;
    //Load all the organ's positions descriptors in the log folder
    std::map<int,OrganPositionDesc> op_desc_map;
    for(const auto &dirit : fs::directory_iterator(fs::path(Logging::log_folder))){
        std::vector<std::string> split;
        misc::split_line(dirit.path().string(),"/",split);
        std::string filename = split.back();
        misc::split_line(filename,"_",split);
        if(split[0] == "morph" && split[1] == "descriptor"){
            std::ifstream ifs(dirit.path().string());
            std::string content{ std::istreambuf_iterator<char>(ifs),
                                 std::istreambuf_iterator<char>() };
            OrganPositionDesc opd;
            opd.from_string(content);
            op_desc_map.emplace(std::stoi(split.back()),opd);
        }
    }
    //-
    //compute the average sparsness
    float average = 0;
    for(const auto& elt : op_desc_map){
        float sparsness = 0;
        for(const auto& elt2 : op_desc_map){
            if(elt.first == elt2.first)
                continue;
            Eigen::VectorXd v = elt.second.getCartDesc();
            Eigen::VectorXd w = elt2.second.getCartDesc();
            sparsness += Novelty::distance_fcts::positional(v,w);
        }
        sparsness /= op_desc_map.size() - 1;
        average += sparsness;
    }
    average /= op_desc_map.size();
    //-

    return average <= threshold;


};

bool Crossbreeding::should_crossbreed(const ioh::MorphGenomeInfoMap &mgim){
    return _trigger(mgim,parameters);
}

void Crossbreeding::selection(int nbr_to_select,std::map<int,NN2CPPNGenome>& genomes){
    std::string robot_library_path = settings::getParameter<settings::String>(parameters,"#robotLibrary").value;
    ioh::MorphGenomeInfoMap morph_genomes_info;
    ioh::load_morph_genomes_info(robot_library_path,morph_genomes_info);
    std::vector<int> list_of_ids = _selection_fct(nbr_to_select,morph_genomes_info);
    ioh::load_morph_genomes<NN2CPPNGenome>(robot_library_path,list_of_ids,genomes);
}
