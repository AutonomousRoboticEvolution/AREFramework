#include "ARE/learning/controller_archive.hpp"

using namespace are;

void ControllerArchive::init(int max_wheels, int max_joints, int max_sensors)
{
    archive.resize(max_wheels+1);
    for(auto& a_w : archive){
        a_w.resize(max_joints+1);
        for(auto& a_j: a_w){
            a_j.resize(max_sensors+1);
        }
    }

    for(auto& w : archive){
        for(auto& j : w){
            for(auto& s : j){
                s.first.reset(new NNParamGenome());
                s.second = 0;
            }
        }
    }
}

void ControllerArchive::reset_fitnesses(){
    for(auto& w : archive){
        for(auto& j : w){
            for(auto& s : j){
                s.second = 0;
            }
        }
    }
}

void ControllerArchive::update(const NNParamGenome::Ptr &genome, double fitness, int wheels, int joints, int sensors){
    if(archive[wheels][joints][sensors].second < fitness)
        archive[wheels][joints][sensors] = std::make_pair(genome,fitness);
}

std::string ControllerArchive::to_string() const{
    std::stringstream sstr;
    for(size_t i = 0; i < archive.size(); i++)
    {
        for(size_t j = 0; j < archive[i].size(); j++)
        {
            for(size_t k = 0; k < archive[i][j].size(); k++)
            {
                sstr << i << "," << j << "," << k << std::endl;
                sstr << archive[i][j][k].first->to_string();
                sstr << archive[i][j][k].second << std::endl;
            }
        }
    }
    return sstr.str();
}

void ControllerArchive::from_file(const std::string& ctrl_arch_file){
    std::ifstream stream(ctrl_arch_file);
    if(!stream)
    {
        std::cerr << "unable to open : " << ctrl_arch_file << std::endl;
        return;
    }

    std::string elt;
    std::string wheel_str,joint_str,sensor_str;
    int w,j,s;
    int nbr_weights,nbr_biases;
    int state = 0;
    double fitness;
    are::NNParamGenome gen;
    while(std::getline(stream,elt)){
        if(state == 0){
            std::stringstream sstr(elt);
            std::getline(sstr,wheel_str,',');
            w = std::stoi(wheel_str);
            std::getline(sstr,joint_str,',');
            j = std::stoi(joint_str);
            std::getline(sstr,sensor_str,',');
            s = std::stoi(sensor_str);
            state = 1;
        }else if(state == 1){
            std::stringstream sstr;
            sstr << elt << std::endl;
            std::getline(stream,elt);
            sstr << elt << std::endl;
            nbr_weights = std::stoi(elt);
            std::getline(stream,elt);
            sstr << elt << std::endl;
            nbr_biases = std::stoi(elt);
            for(int i = 0; i < nbr_weights; i++){
                std::getline(stream,elt);
                sstr << elt << std::endl;
            }
            for(int i = 0; i < nbr_biases; i++){
                std::getline(stream,elt);
                sstr << elt << std::endl;
            }
            gen.from_string(sstr.str());
            state = 2;
        }else if (state == 2){
            fitness = std::stod(elt);
            archive[w][j][s] = std::make_pair(std::make_shared<are::NNParamGenome>(gen),fitness);
            state = 0;
        }
    }
}
