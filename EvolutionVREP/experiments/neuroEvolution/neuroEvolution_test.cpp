#include "basicEA.hpp"
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
    parameters.emplace("#populationSize",new are_set::Integer(100));
    parameters.emplace("#mutationType",new are_set::Integer(are::mutators::type::GAUSSIAN));
    parameters.emplace("#mutationRate",new are_set::Double(0.4));
    parameters.emplace("#mutationParam",new are_set::Double(1.));
    parameters.emplace("#crossoverType",new are_set::Integer(are::crossovers::type::SBX));
    parameters.emplace("#crossoverParameter",new are_set::Double(0.1));

    parameters.emplace("#maxWeight",new are_set::Double(5.));
    parameters.emplace("#verbose",new are_set::Boolean(true));
    parameters.emplace("#tournamentSize",new are_set::Integer(20));
    parameters.emplace("#NNType",new are_set::Integer(are_set::nnType::FFNN));
    parameters.emplace("#NbrInputNeurones",new are_set::Integer(1));
    parameters.emplace("#NbrHiddenNeurones",new are_set::Integer(0));
    parameters.emplace("#NbrOutputNeurones",new are_set::Integer(1));

    std::random_device rd;
    int seed = rd();
    misc::RandNum::Ptr rngen (new misc::RandNum(seed));

    are::BasicEA ea(std::make_shared<are_set::ParametersMap>(parameters));
    ea.set_randomNum(rngen);
    ea.init();
    std::vector<are::Individual::Ptr> pop;
    int gen_size = std::dynamic_pointer_cast<are::NNParamGenome>(ea.get_population()[0]->get_ctrl_genome())->get_full_genome().size();
    std::vector<double> zeros(0,gen_size);
    double best_fit = 1e14;
    std::vector<double> best_gen;
    int eval = 0;
    while(best_fit > 0.5){
        pop = ea.get_population();
        std::vector<double> genome;
        double fit,norm_fit;
        double instant_best_fit = 1e14;
        double avg_fit = 0;
        for(are::Individual::Ptr& ind : pop){
            genome = std::dynamic_pointer_cast<are::NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();

            fit = rastrigin(10,genome);
            avg_fit += fit;
            if(fit > 200) fit =  200;
            norm_fit = 1 - fit/200.;
            std::dynamic_pointer_cast<are::NN2Individual>(ind)->set_final_position({norm_fit,0,0}); //fake position because mandatory for novelty in this experiment
            ind->setObjectives({norm_fit});
//            std::cout << "Fitness : " << fit << std::endl;
            if(fit < best_fit){
                best_fit = fit;
                best_gen = genome;
            }
            if(fit < instant_best_fit){
                instant_best_fit = fit;
                best_gen = genome;
            }

            eval++;
        }
        std::cout << "best fitness : " << instant_best_fit << " avg fitness : " << avg_fit/pop.size() << std::endl;
        std::cout << "best genome : ";
        for(const double& g : best_gen)
                std::cout << g << " ; ";
        std::cout << std::endl;
        ea.epoch();
        ea.init_next_pop();
    }
    std::cout << "Solution found : "  << best_fit << " for target value : "  << rastrigin(10,zeros) << " in " << eval << " evaluations" << std::endl;

}
