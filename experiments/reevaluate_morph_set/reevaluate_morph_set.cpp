#include "reevaluate_morph_set.hpp"


using namespace are;
namespace  fs = boost::filesystem;


void ReevaluateMorphSet::init(){


    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    load_population(folder_to_load);
}

void ReevaluateMorphSet::setObjectives(size_t indIdx, const std::vector<double> &objectives){
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

bool ReevaluateMorphSet::update(const Environment::Ptr & env){
    endEvalTime = hr_clock::now();
    numberEvaluation++;

    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());
        std::dynamic_pointer_cast<NN2Individual>(ind)->set_trajectory(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_trajectory());
    }

    return true;
}




void ReevaluateMorphSet::load_population(const std::string &folder){
    std::string filename;
    std::vector<std::string> split_str;
    std::map<int,NNParamGenome> ctrl_map;
    std::map<int,CPPNGenome> morph_map;
    NEAT::Genome neat_genome;
    NNParamGenome ctrl_genome;
    for(const auto &dirit : fs::directory_iterator(fs::path(folder))){
        filename = dirit.path().string();
        boost::split(split_str,filename,boost::is_any_of("/"));
        boost::split(split_str,split_str.back(),boost::is_any_of("_"));
        if(split_str[1] == "morph"){
            neat_genome = NEAT::Genome(filename.c_str());
            morph_map.emplace(std::stoi(split_str.back()),CPPNGenome(neat_genome));
        }
        else if(split_str[1] == "ctrl"){

            ctrl_genome = NNParamGenome(randomNum,parameters);

            std::ifstream logFileStream;
            logFileStream.open(filename);
            std::string line;
            std::getline(logFileStream,line);
            int nbr_weights = std::stoi(line);
            std::getline(logFileStream,line);
            int nbr_bias = std::stoi(line);

            std::vector<double> weights;
            for(int i = 0; i < nbr_weights; i++){
                std::getline(logFileStream,line);
                weights.push_back(std::stod(line));
            }
            ctrl_genome.set_weights(weights);

            std::vector<double> biases;
            for(int i = 0; i < nbr_bias; i++){
                std::getline(logFileStream,line);
                biases.push_back(std::stod(line));
            }
            ctrl_genome.set_biases(biases);
            ctrl_map.emplace(std::stoi(split_str.back()),ctrl_genome);
        }
    }

    assert(ctrl_map.size() == morph_map.size());
    population.clear();
    for(const auto &morph : morph_map){
        Individual::Ptr ind(new NN2Individual(std::make_shared<CPPNGenome>(morph.second),
                                              std::make_shared<NNParamGenome>(ctrl_map[morph.first])));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }

}
