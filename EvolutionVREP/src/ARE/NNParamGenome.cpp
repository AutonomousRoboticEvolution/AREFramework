#include "ARE/NNParamGenome.hpp"

using namespace are;

mutators_t mutators::uniform = [](std::vector<double>& genome, double* param, const misc::RandNum::Ptr& rn){
    for(double& gene : genome){
        if(rn->randDouble(0,1) < param[0]){
            gene = gene + rn->randDouble(param[1],param[2]);
        }
    }
};

mutators_t mutators::gaussian = [](std::vector<double>& genome, double* param, const misc::RandNum::Ptr& rn){
    for(double& gene : genome){
        if(rn->randDouble(0,1) < param[0]){
            gene = gene + rn->normalDist(0,param[1]);
            if(gene > param[2])
                gene = param[2];
            if (gene < param[3])
                gene = param[3];
        }
    }
};

mutators_t mutators::polynomial = [](std::vector<double>& genome, double* param, const misc::RandNum::Ptr& rn){
    for(double& gene : genome){
        if(rn->randDouble(0,1) < param[0])
        {
            double ri = rn->randDouble(0,1);
            double delta_i = ri < 0.5 ? std::pow(2.0*ri, 1.0/(param[1] + 1.0)) - 1.0 :
                1.0 - std::pow(2.0*(1.0 - ri), 1.0/(param[1] + 1.0));

            gene = gene + delta_i;
            if(gene > param[2])
                gene = param[2];
            if (gene < param[3])
                gene = param[3];
        }
    }

};

crossover_t crossovers::sbx = [](const std::vector<double> &p1, const std::vector<double> &p2,
        std::vector<double> &c1,std::vector<double>& c2, double* param, const misc::RandNum::Ptr &rn){
    c1.resize(p1.size());
    c2.resize(p2.size());
    for(size_t i = 0; i < p1.size(); i++){
        double y1 = std::min(p1[i], p2[i]);
        double y2 = std::max(p1[i], p2[i]);
        const double lbound = param[0];
        const double ubound = param[1];
        const double eta = param[2];
        if(fabs(y1-y2) > std::numeric_limits<double>::epsilon()){
            double rand = rn->randDouble(0,1);
            double beta = 1.0 + 2.0*(y1 - lbound)/(y2 - y1);
            double alpha = 2.0 - pow(beta, -(eta + 1.0));
            double betaq = 0;
            if(rand <= (1.0/alpha))
                betaq = pow((rand * alpha), (1.0 / (eta + 1.0)));
            else
                betaq = pow ((1.0 / (2.0 - rand * alpha)) , (1.0 / (eta + 1.0)));
            double x1 = 0.5*((y1 + y2) - betaq*(y2-y1));

            beta = 1.0 + 2.0*(ubound - y2)/(y2 - y1);
            alpha = 2.0 - pow(beta, -(eta + 1.0));
            if(rand <= (1.0/alpha))
                betaq = pow((rand * alpha), (1.0 / (eta + 1.0)));
            else
                betaq = pow ((1.0 / (2.0 - rand * alpha)) , (1.0 / (eta + 1.0)));
            double x2 = 0.5*((y1 + y2) - betaq*(y2-y1));

            if(x1 < lbound) x1 = lbound;
            else if(x1 > ubound) x1 = ubound;
            if(x2 < lbound) x2 = lbound;
            else if(x2 > ubound) x2 = ubound;

            if(rn->randInt(0,1)){
                c1[i] = x1;
                c2[i] = x2;
            }else{
                c1[i] = x2;
                c2[i] = x1;
            }
        }
        else{
            c1[i] = y1;
            c2[i] = y2;
        }
    }
};

void NNParamGenome::mutate(){
    int mutation_type = settings::getParameter<settings::Integer>(parameters,"#mutationType").value;
    double mutation_rate = settings::getParameter<settings::Double>(parameters,"#mutationRate").value;
    double max_weight = settings::getParameter<settings::Double>(parameters,"#maxWeight").value;
    std::vector<double> genome = get_full_genome();
    if(mutation_type = mutators::type::UNIFORM)
    {
        double param[3] {mutation_rate,max_weight,-max_weight};
        mutators::uniform(genome,param,randomNum);
    }else if(mutation_type = mutators::type::GAUSSIAN){
        double mutation_param = settings::getParameter<settings::Double>(parameters,"#mutationParam").value;
        double param[4] {mutation_rate,mutation_param,max_weight,-max_weight};
        mutators::gaussian(genome,param,randomNum);
    }else if(mutation_type = mutators::type::POLYNOMIAL){
        double mutation_param = settings::getParameter<settings::Double>(parameters,"#mutationParam").value;
        double param[4] {mutation_rate,mutation_param,max_weight,-max_weight};
        mutators::polynomial(genome,param,randomNum);
    }

    for(int i = 0; i < weights.size(); i++)
        weights[i] = genome[i];
    for(int i = 0; i < biases.size(); i++)
        biases[i] = genome[i + weights.size()];
}

void NNParamGenome::crossover(const Genome::Ptr &partner, Genome::Ptr child1, Genome::Ptr child2){
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
    sstr << weights.size() << std::endl;
    sstr << biases.size() << std::endl;
    for(double w : weights)
        sstr << w << std::endl;
    for(double b : biases)
        sstr << b << std::endl;
    return sstr.str();
}

void NNParamGenome::from_string(const std::string &gen_str){
    std::vector<std::string> split_str;
    boost::split(split_str,gen_str,boost::is_any_of("\n"));
    int nbr_weights = std::stoi(split_str[0]);
    int nbr_bias = std::stoi(split_str[1]);

    weights.clear();
    for(int i = 2; i < nbr_weights+2; i++)
        weights.push_back(std::stod(split_str[i]));

    biases.clear();
    for(int i = nbr_weights; i < nbr_weights + nbr_bias; i++)
        biases.push_back(std::stod(split_str[i]));
}

void NNParamGenome::from_file(const std::string &filename){
    std::ifstream logFileStream;
    logFileStream.open(filename);
    std::string line;
    std::getline(logFileStream,line);
    int nbr_weights = std::stoi(line);
    std::getline(logFileStream,line);
    int nbr_bias = std::stoi(line);

    weights.clear();
    for(int i = 0; i < nbr_weights; i++){
        std::getline(logFileStream,line);
        weights.push_back(std::stod(line));
    }

    biases.clear();
    for(int i = 0; i < nbr_bias; i++){
        std::getline(logFileStream,line);
        biases.push_back(std::stod(line));
    }
}

void NNParamGenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "genome_" << generation << "_" << i;
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<NNParamGenome>(
                             ea->get_population()[i]->get_ctrl_genome()
                             )->to_string();
        logFileStream.close();
    }
}
