#include "grid_archive.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Usage: arg1 path to meim log folder " << std::endl;
        return 1;
    }

    std::string log_folder = argv[1];
    std::ifstream fit_file(log_folder + "/fitness.csv");
    std::vector<std::array<double,8>> population_data;
    std::vector<std::array<double,4>> fit_data;
    for(std::string line; std::getline(fit_file,line); ){
        std::stringstream sstream(line);
        std::string item;
        std::array<double,4> vals;
        int idx = 0;
        while(std::getline(sstream,item,',')){
            vals[idx] = stod(item);
            idx++;
        }
        fit_data.push_back(vals);
    }

    fit_file.close();
    std::vector<std::array<double,9>> desc_data;
    std::ifstream desc_file(log_folder + "/morph_features.csv");
    for(std::string line; std::getline(desc_file,line); ){
        std::stringstream sstream(line);
        std::string item;
        std::array<double,9> desc_vals;
        int idx = 0;
        while(std::getline(sstream,item,',')){
            desc_vals[idx] = stod(item);
            idx++;
        }
        desc_data.push_back(desc_vals);
    }
    desc_file.close();
    if(desc_data.size() != fit_data.size()){
        std::cerr << "Mismatch between number of fitness and descriptor entries!" << std::endl;
        std::cerr << "Fitness entries: " << fit_data.size() << " Descriptor entries: " << desc_data.size() << std::endl;
        return 1;
    }
    for(size_t i = 0; i < fit_data.size(); i++){
        std::array<double,8> full_data;
        full_data = {fit_data[i][0],fit_data[i][3],desc_data[i][1],desc_data[i][2],desc_data[i][3],desc_data[i][5],desc_data[i][6],desc_data[i][7]};
        population_data.push_back(full_data);
    }
    are::GridArchive<std::array<double,8>>::comparator_t comp = [](const std::array<double,8> &a, const std::array<double,8> &b){
            return a[1] > b[1];
    };
    are::GridArchive<std::array<double,8>> archive({6,6,12,12,12,12},{{0.4,1},{0.4,1},{0,1},{0,1},{0,1},{0,1}},comp);
    for(const auto &data: population_data){
        archive.add_solution(data,{data[2],data[3],data[4],data[5],data[6],data[7]});
    }
    std::cout << "Final archive size: " << archive.size() << std::endl;
    std::ofstream archive_file(log_folder + "/grid_archive.csv");
    for(const auto &cell: archive.get_solutions())
        archive_file << cell[0]  << ",";
    archive_file.close();
    return 0;
}
