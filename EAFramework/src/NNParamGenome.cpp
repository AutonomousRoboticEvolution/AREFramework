#include "ARE/NNParamGenome.hpp"

using namespace are;



void NNParamGenome::mutate(){
    int mutation_type = settings::getParameter<settings::Integer>(parameters,"#mutationType").value;
    double mutation_rate = settings::getParameter<settings::Double>(parameters,"#mutationRate").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;
    std::vector<double> genome = get_full_genome();
    if(mutation_type == mutators::type::UNIFORM)
    {
        double param[3] {mutation_rate,max_weight,-max_weight};
        mutators::uniform(genome,param,randomNum);
    }else if(mutation_type == mutators::type::GAUSSIAN){
        double mutation_param = settings::getParameter<settings::Double>(parameters,"#mutationParam").value;
        double param[4] {mutation_rate,mutation_param,max_weight,-max_weight};
        mutators::gaussian(genome,param,randomNum);
    }else if(mutation_type == mutators::type::POLYNOMIAL){
        double mutation_param = settings::getParameter<settings::Double>(parameters,"#mutationParam").value;
        double param[4] {mutation_rate,mutation_param,max_weight,-max_weight};
        mutators::polynomial(genome,param,randomNum);
    }

    for(size_t i = 0; i < weights.size(); i++)
        weights[i] = genome[i];
    for(size_t i = 0; i < biases.size(); i++)
        biases[i] = genome[i + weights.size()];
}

void NNParamGenome::crossover(const Genome::Ptr &partner, Genome::Ptr child){
    int x_type = settings::getParameter<settings::Integer>(parameters,"#crossoverType").value;
    double x_param = settings::getParameter<settings::Double>(parameters,"#crossoverParameter").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;

    if(x_type == crossovers::type::NO_CROSSOVER){
        std::dynamic_pointer_cast<NNParamGenome>(child)->set_weights(std::dynamic_pointer_cast<NNParamGenome>(partner)->get_weights());
        std::dynamic_pointer_cast<NNParamGenome>(child)->set_biases(std::dynamic_pointer_cast<NNParamGenome>(partner)->get_biases());
        return;
    }

    std::vector<double> p1 = get_full_genome();
    std::vector<double> p2 = std::dynamic_pointer_cast<NNParamGenome>(partner)->get_full_genome();
    std::vector<double> c1,c2;

    if(x_type == crossovers::type::SBX){
        double param[3] {-max_weight,max_weight,x_param};
        crossovers::sbx(p1,p2,c1,c2,param,randomNum);
    }

    std::vector<double> w,b;
    w.insert(w.begin(),c1.begin(),c1.begin()+weights.size());
    b.insert(b.begin(),c1.begin()+weights.size(),c1.end());

    std::dynamic_pointer_cast<NNParamGenome>(child)->set_weights(w);
    std::dynamic_pointer_cast<NNParamGenome>(child)->set_biases(b);
}

void NNParamGenome::symmetrical_crossover(const Genome::Ptr &partner, Genome::Ptr child1, Genome::Ptr child2){
    int x_type = settings::getParameter<settings::Integer>(parameters,"#crossoverType").value;
    double x_param = settings::getParameter<settings::Double>(parameters,"#crossoverParameter").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;

    if(x_type == crossovers::type::NO_CROSSOVER){
        std::dynamic_pointer_cast<NNParamGenome>(child1)->set_weights(std::dynamic_pointer_cast<NNParamGenome>(partner)->get_weights());
        std::dynamic_pointer_cast<NNParamGenome>(child1)->set_biases(std::dynamic_pointer_cast<NNParamGenome>(partner)->get_biases());
        std::dynamic_pointer_cast<NNParamGenome>(child2)->set_weights(weights);
        std::dynamic_pointer_cast<NNParamGenome>(child2)->set_biases(biases);
        return;
    }

    std::vector<double> p1 = get_full_genome();
    std::vector<double> p2 = std::dynamic_pointer_cast<NNParamGenome>(partner)->get_full_genome();
    std::vector<double> c1,c2;

    if(x_type == crossovers::type::SBX){
        double param[3] {-max_weight,max_weight,x_param};
        crossovers::sbx(p1,p2,c1,c2,param,randomNum);
    }

    std::vector<double> w1,w2,b1,b2;
    w1.insert(w1.begin(),c1.begin(),c1.begin()+weights.size());
    b1.insert(b1.begin(),c1.begin()+weights.size(),c1.end());
    w2.insert(w2.begin(),c2.begin(),c2.begin()+weights.size());
    b2.insert(b2.begin(),c2.begin()+weights.size(),c2.end());


    std::dynamic_pointer_cast<NNParamGenome>(child1)->set_weights(w1);
    std::dynamic_pointer_cast<NNParamGenome>(child1)->set_biases(b1);

    std::dynamic_pointer_cast<NNParamGenome>(child2)->set_weights(w2);
    std::dynamic_pointer_cast<NNParamGenome>(child2)->set_biases(b2);


}

std::string NNParamGenome::to_string() const{
    std::stringstream sstr;
    sstr << nn_type << " " << nbr_input << " " << nbr_hidden << " " << nbr_output << std::endl;
    sstr << weights.size() << std::endl;
    sstr << biases.size() << std::endl;
    for(double w : weights)
        sstr << w << std::endl;
    for(double b : biases)
        sstr << b << std::endl;
    return sstr.str();
}

void NNParamGenome::from_string(const std::string &gen_str){
    std::vector<std::string> split_str,split_str2;
    boost::split(split_str,gen_str,boost::is_any_of("\n"),boost::token_compress_on); // boost::token_compress_on means it will ignore any empty lines (where there is adjacent newline charaters)
    boost::split(split_str2,split_str[0],boost::is_any_of(" "));

    nn_type = std::stoi(split_str2[0]);
    nbr_input = std::stoi(split_str2[1]);
    nbr_hidden = std::stoi(split_str2[2]);
    nbr_output = std::stoi(split_str2[3]);

    int nbr_weights = std::stoi(split_str[1]);
    int nbr_bias = std::stoi(split_str[2]);

    weights.clear();
    for(int i = 3; i < nbr_weights+3; i++)
        weights.push_back(std::stod(split_str[i]));

    biases.clear();
    for(int i = nbr_weights + 3; i < nbr_weights + nbr_bias + 3; i++)
        biases.push_back(std::stod(split_str[i]));
}




void NNParamGenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "ctrlGenome_" << generation << "_" << i;
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        auto &ind = ea->get_population()[i];
        if(ind->getObjectives()[0] >= objective_threshold)
            logFileStream << std::dynamic_pointer_cast<NNParamGenome>(
                                 ind->get_ctrl_genome())->to_string();

        logFileStream.close();
    }
}
