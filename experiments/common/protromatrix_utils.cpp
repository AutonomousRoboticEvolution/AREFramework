//
// Created by edgar on 26/04/22.
//
#include "protomatrix_utils.hpp"

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
std::vector<std::vector<double>> protomatrix::random_matrix(std::vector<std::vector<double>> matrix_4d){
    matrix_4d.resize(6);
    const float mutation_parameter = 1.0;
    for(int i = 0; i < matrix_4d.size(); i++) {
        for(int j = 0; j < 1331; j++) { /// \todo EB: Hardcoded value!
            double rand_number = std::uniform_real_distribution<>(-mutation_parameter,mutation_parameter)(nn2::rgen_t::gen);
            matrix_4d.at(i).push_back(rand_number);
        }
    }
    return matrix_4d;
}

std::vector<std::vector<double>> protomatrix::mutate_matrix(std::vector<std::vector<double>> matrix_4d){
    std::vector<std::vector<double>> mutated_matrix_4d = matrix_4d;
    const float mutation_rate = 0.1;
    const float mutation_parameter = 0.5;
    const int mutation_type = 0;
    for(int i = 0; i < matrix_4d.size(); i++){
        for(int j = 0; j < matrix_4d.at(0).size(); j++){
            if(std::uniform_real_distribution<>(0,1)(nn2::rgen_t::gen) < mutation_rate) {
                if(i == 1){
                    if(matrix_4d.at(i).at(j) == -1)
                        mutated_matrix_4d.at(i).at(j) = 1.;
                    else if(matrix_4d.at(i).at(j) == 1)
                        mutated_matrix_4d.at(i).at(j) = -1.;
                    else {
                        std::cerr << "We can't be here! " << __func__ << std::endl;
                        assert(false);
                    }
                } else {
                    if (mutation_type == 0) { // Uniform distribution
                        double temp_variable;
                        double rand_number = std::uniform_real_distribution<>(-mutation_parameter, mutation_parameter)(
                                nn2::rgen_t::gen);
                        temp_variable = rand_number + matrix_4d.at(i).at(j);
                        if (temp_variable < -1.0)
                            temp_variable = -1.0;
                        else if (temp_variable > 1.0)
                            temp_variable = 1.0;
                        mutated_matrix_4d.at(i).at(j) = temp_variable;
                    } else if (mutation_type == 1) { // Polynomial mutation
                        const float eta_m = mutation_parameter;
                        assert(eta_m != -1.0f);
                        float ri = std::uniform_real_distribution<>(0, 1)(nn2::rgen_t::gen);
                        float delta_i = ri < 0.5 ?
                                        pow(2.0 * ri, 1.0 / (eta_m + 1.0)) - 1.0 :
                                        1 - pow(2.0 * (1.0 - ri), 1.0 / (eta_m + 1.0));
                        assert(!std::isnan(delta_i));
                        assert(!std::isinf(delta_i));
                        float temp_variable = matrix_4d.at(i).at(j) + delta_i;
                        if (temp_variable < -1.0)
                            temp_variable = -1.0;
                        else if (temp_variable > 1.0)
                            temp_variable = 1.0;
                        mutated_matrix_4d.at(i).at(j) = temp_variable;
                    } else {
                        std::cerr << "Wrong mutation_type: " << mutation_type << " in " << __func__ << std::endl;
                        assert(false);
                    }
                }
            }
        }
    }
    return mutated_matrix_4d;
}
std::vector<std::vector<double>> protomatrix::crossover_matrix(std::vector<std::vector<double>> first_parent_4d,std::vector<std::vector<double>> second_parent_4d){
    std::vector<std::vector<double>> child_matrix_4d;
    child_matrix_4d = first_parent_4d;
    for(int i = 0; i < first_parent_4d.size(); i++){
        for(int j = 0; j < first_parent_4d.at(0).size(); j++){
            double first_parent_output = first_parent_4d.at(i).at(j);
            double second_parent_output = second_parent_4d.at(i).at(j);
            double result = (first_parent_output + second_parent_output)/2.;
            child_matrix_4d.at(i).at(j) = result;
        }
    }
    return child_matrix_4d;
}