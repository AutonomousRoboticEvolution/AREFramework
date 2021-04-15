#include "MB_NIPES.hpp"
#include <random>

namespace are_set = are::settings;

inline double rastrigin(double A, std::vector<double> X){
    double res = 0;
    for(const double &x : X)
        res += x*x - A*std::cos(2*M_PI*x);

    return A*static_cast<double>(X.size()) + res;
}

inline double sphere(std::vector<double> X){
    double res = 0;
    for(const double &x : X)
        res += x*x;
    return res;
}


int main()
{

    are_set::ParametersMap parameters;
    parameters.emplace("#populationSize",new are_set::Integer(200));
    parameters.emplace("#MaxWeight",new are_set::Float(5.));
    parameters.emplace("#verbose",new are_set::Boolean(true));
    parameters.emplace("#kValue",new are_set::Integer(15));
    parameters.emplace("#noveltyThreshold", new are_set::Double(0.9));
    parameters.emplace("#archiveAddingProb", new are_set::Double(0.4));
    parameters.emplace("#NNType",new are_set::Integer(are_set::nnType::FFNN));
    parameters.emplace("#NbrInputNeurones",new are_set::Integer(1));
    parameters.emplace("#NbrHiddenNeurones",new are_set::Integer(0));
    parameters.emplace("#NbrOutputNeurones",new are_set::Integer(1));
    parameters.emplace("#instanceType",new are_set::Integer(0));

    parameters.emplace("#GPUCBParam", new are_set::Double(0)); //useless
    parameters.emplace("#observationType",new are_set::Integer(are_set::obsType::FINAL_POS)); //useless



    parameters.emplace("#lengthOfStagnation",new are_set::Integer(100));
    parameters.emplace("#CMAESStep",new are_set::Double(1));
    parameters.emplace("#FTarget",new are_set::Double(0.01));
    parameters.emplace("#elitistRestart",new are_set::Boolean(true));
    parameters.emplace("#noveltyRatio",new are_set::Double(0));
    parameters.emplace("#noveltyDecrement",new are_set::Double(0.05));
    parameters.emplace("#populationStagnationThreshold",new are_set::Float(0.01));
    parameters.emplace("#arenaSize",new are_set::Double(2.));
    parameters.emplace("#withRestart",new are_set::Boolean(false));
    parameters.emplace("#incrPop",new are_set::Boolean(false));
    parameters.emplace("#uncertaintyThreshold",new are_set::Double(0.3));

    std::random_device rd;
    int seed = rd();
    misc::RandNum::Ptr rngen (new misc::RandNum(seed));

    are::MB_NIPES nipes(std::make_shared<are_set::ParametersMap>(parameters));
    nipes.set_randomNum(rngen);
    nipes.init();
    std::vector<are::Individual::Ptr> pop;
    int gen_size = std::dynamic_pointer_cast<are::NNParamGenome>(nipes.get_population()[0]->get_ctrl_genome())->get_full_genome().size();
    std::vector<double> zeros(0,gen_size);
    std::cout << "target value : " << rastrigin(10,zeros) << std::endl;
    double best_fit = 1e14;
    std::vector<double> best_gen;
    int eval = 0;
    are::MazeEnv::Ptr env(new are::MazeEnv);

    while(best_fit > 0.5){
        pop = nipes.get_population();
        std::vector<double> genome;
        double fit, norm_fit;
        for(are::Individual::Ptr& ind : pop){
            genome = std::dynamic_pointer_cast<are::NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();

            fit = rastrigin(10,genome);
            if(fit > 200) fit =  200;
            norm_fit = 1. - fit/200.;
            std::dynamic_pointer_cast<are::sim::NN2Individual>(ind)->set_final_position({genome[0],genome[1],genome[2]}); //fake position because mandatory for novelty in this experiment
            ind->setObjectives({norm_fit});
            if(fit < best_fit){
                best_fit = fit;
                best_gen = genome;
            }
//            std::cout << "Fitness : " << fit << std::endl;
            nipes.update(env);

            eval++;
        }
        std::cout << "best fitness : " << best_fit << std::endl;
        for(const double& g : best_gen)
                std::cout << g << " ; ";
        std::cout << std::endl;
        nipes.epoch();
        nipes.init_next_pop();
    }

    std::cout << "Solution found : "  << best_fit << " for target value : "  << rastrigin(10,zeros) << " in " << eval << " evaluations" << std::endl;


}
