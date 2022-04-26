//
// Created by edgar on 26/04/22.
//
#include "protomatrix_utils.h"

using namespace are;

std::vector<std::vector<double>> protomatrix::load_robot_matrix(std::string filepath){
    std::vector<std::vector<double>> matrix_4d;
    // Create an input filestream
    std::ifstream myFile(filepath);
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");
    std::string line;
    std::vector<std::string> split_str;
    int output_counter = 0;
    matrix_4d.resize(6);
    while(std::getline(myFile, line)){
        // Create a stringstream of the current line
        std::stringstream ss(line);
        // Keep track of the current column index
        boost::split(split_str,line,boost::is_any_of(","),boost::token_compress_on); // boost::token_compress_on means it will ignore any empty lines (where there is adjacent newline charaters)
        for(int i = 1; i < split_str.size()-1; i++){
            matrix_4d.at(output_counter).push_back(std::stod(split_str.at(i)));
        }
        output_counter++;
    }
    // Close file
    myFile.close();
    return matrix_4d;
}
std::vector<std::vector<double>> protomatrix::mutate_matrix(std::vector<std::vector<double>> matrix_4d){

}
std::vector<std::vector<double>> protomatrix::crossover_matrix(std::vector<std::vector<double>> first_parent_4d,std::vector<std::vector<double>> second_parent_4d){

}