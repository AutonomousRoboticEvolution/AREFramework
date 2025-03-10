#include "nn_analysis.hpp"

void split(const std::string &str,const std::string &delim,std::vector<std::string> &values){
    values.clear();
    size_t pos = 0;
    std::string l = str;
    while ((pos = l.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = l.substr(0, pos);
        values.push_back(p);
        l = l.substr(pos + 1);
        pos = 0;
    }

    if (!l.empty()) {
        //        split_line(l,delim,values);
        values.push_back(l);
    }
}

bool read_parameters(const std::string &filename,int& nn_type,int &nbr_inputs,int &nbr_hidden,int &nbr_outputs,std::vector<double> &weights,std::vector<double> &biases){
    std::ifstream logFileStream;
    logFileStream.open(filename);
    if(!logFileStream){
        std::cerr << "Genome::from_file - unable to open " << filename << std::endl;
        return false;
    }
    std::string str = "",line;
    while(std::getline(logFileStream,line))
        str = str + line + "\n";

    std::vector<std::string> split_str,split_str2;
    split(str,"\n",split_str); // boost::token_compress_on means it will ignore any empty lines (where there is adjacent newline charaters)
    split(split_str[0]," ",split_str2);

    nn_type = std::stoi(split_str2[0]);
    nbr_inputs = std::stoi(split_str2[1]);
    nbr_hidden = std::stoi(split_str2[2]);
    nbr_outputs = std::stoi(split_str2[3]);

    int nbr_weights = std::stoi(split_str[1]);
    int nbr_bias = std::stoi(split_str[2]);

    weights.clear();
    for(int i = 3; i < nbr_weights+3; i++)
        weights.push_back(std::tanh(std::stod(split_str[i])));

    biases.clear();
    for(int i = nbr_weights + 3; i < nbr_weights + nbr_bias + 3; i++)
        biases.push_back(std::tanh(std::stod(split_str[i])));
    return true;
}

std::string io_to_string(const std::vector<double> inputs,const std::vector<double> &outputs){
    std::stringstream sstr;
    for(const double& i: inputs){
        sstr << i << ";";
    }
    for(const double& o: outputs){
        sstr << o << ";";
    }
    std::string output = sstr.str();
    output.erase(output.end() -1); //remove the last ;
    return output;
}

int main(int argc, char** argv){
    if(argc < 3){
        std::cerr << "usage: path to controller genome from the ARE framework containing the parameter of the neural network" << std::endl;
        return 1;
    }

    //read the nn parameters from file
    int nn_type, nbr_inputs, nbr_outputs, nbr_hidden;
    std::vector<double> weights, biases;
    if(!read_parameters(argv[1],nn_type,nbr_inputs,nbr_hidden,nbr_outputs,weights,biases)){
        std::cerr << "Unable to open file : " << argv[1] << std::endl;
        return 2;
    }
    float step = std::stof(argv[2]);

    if(nn_type == FFNN)
        analyse<ffnn_t>(nbr_inputs,nbr_outputs,nbr_hidden,weights,biases,step);
    else if(nn_type == RNN)
        analyse<rnn_t>(nbr_inputs,nbr_outputs,nbr_hidden,weights,biases,step);
    else if(nn_type == ELMAN)
        analyse<elman_t>(nbr_inputs,nbr_outputs,nbr_hidden,weights,biases,step);
//    else if(nn_type == ELMAN_CPG){
//        if(argc < 4){
//            std::cerr << "The neural network include CPGs, please provide the joint substrates as last argument\n Example: -1;0 for two oscillators connected in serie" << std::endl;
//            return 1;
//        }
//        std::vector<std::string> joint_subs_str;
//        split(std::string(argv[3]),";",joint_subs_str);
//        std::vector<int> joint_subs;
//        for(const auto &val: joint_subs_str)
//            joint_subs.push_back(std::stoi(val));
//        analyse_cpg<elman_cpg_t>(nbr_inputs,nbr_outputs,nbr_hidden,joint_subs,weights,biases,step);
//    }
    else if(nn_type == CPG){
        if(argc < 4){
            std::cerr << "The neural network include CPGs, please provide the joint substrates as last argument\n Example: -1;0 for two oscillators connected in serie" << std::endl;
            return 1;
        }

        std::vector<std::string> joint_subs_str;
        split(std::string(argv[3]),";",joint_subs_str);
        std::vector<int> joint_subs;
        for(const auto &val: joint_subs_str)
            joint_subs.push_back(std::stoi(val));
        analyse_cpg<cpg_t>(nbr_inputs,nbr_outputs,nbr_hidden,joint_subs,weights,biases,step);
    }
    else if(nn_type == FF_CPG){
        if(argc < 4){
            std::cerr << "The neural network include CPGs, please provide the joint substrates as last argument\n Example: -1;0 for two oscillators connected in serie" << std::endl;
            return 1;
        }

        std::vector<std::string> joint_subs_str;
        split(std::string(argv[3]),";",joint_subs_str);
        std::vector<int> joint_subs;
        for(const auto &val: joint_subs_str)
            joint_subs.push_back(std::stoi(val));
        analyse_cpg<ff_cpg_t>(nbr_inputs,nbr_outputs,nbr_hidden,joint_subs,weights,biases,step);
    }
    else if(nn_type == FCP)
        analyse<fcp_t>(nbr_inputs,nbr_outputs,nbr_hidden,weights,biases,step);

    return 0;
}
