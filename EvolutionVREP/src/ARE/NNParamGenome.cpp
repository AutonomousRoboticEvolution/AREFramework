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
