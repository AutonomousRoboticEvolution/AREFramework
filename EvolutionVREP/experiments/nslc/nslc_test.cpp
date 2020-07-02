#include "NSLC.hpp"
#include <random>

namespace are_set = are::settings;

inline double rastrigin(double A, std::vector<double> x){
    return A*static_cast<double>(x.size()) + std::accumulate(x.begin(),x.end(),0,[&](double a, double b)->double{
        return  a + b*b - A*std::cos(2*M_PI*b);
    });
}


int main()
{

    are_set::ParametersMap parameters;
    parameters.emplace("#populationSize",new are_set::Integer(20));
    parameters.emplace("#mutationType",new are_set::Integer(are::mutators::type::GAUSSIAN));
    parameters.emplace("#mutationRate",new are_set::Double(0.4));
    parameters.emplace("#mutationParam",new are_set::Double(3.));
    parameters.emplace("#maxWeight",new are_set::Double(5.));
    parameters.emplace("#verbose",new are_set::Boolean(true));
    parameters.emplace("#kValue",new are_set::Integer(15));
    parameters.emplace("#noveltyThreshold", new are_set::Double(0.9));
    parameters.emplace("#archiveAddingProb", new are_set::Double(0.4));
    parameters.emplace("#NNType",new are_set::Integer(are_set::nnType::FFNN));
    parameters.emplace("#NbrInputNeurones",new are_set::Integer(1));
    parameters.emplace("#NbrHiddenNeurones",new are_set::Integer(2));
    parameters.emplace("#NbrOutputNeurones",new are_set::Integer(1));



    parameters.emplace("#useNSGA2Diversity",new are_set::Boolean(false));


    std::random_device rd;
    int seed = rd();
    misc::RandNum::Ptr rngen (new misc::RandNum(seed));

    are::NSLC nslc(std::make_shared<are_set::ParametersMap>(parameters));
    nslc.set_randomNum(rngen);
    nslc.init();
    nslc.set_obj_bounds({{0,-200},{1,0}});
    int max_iter = 1000;
    std::vector<are::Individual::Ptr> pop;
    for(int iter = 0; iter < max_iter; iter++){
        pop = nslc.get_population();
        std::vector<double> genome;
        double fit,norm_fit;
        double best_fit = 1e14;
        for(are::Individual::Ptr& ind : pop){
            genome = std::dynamic_pointer_cast<are::NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();

            fit = rastrigin(10,genome);
            if(fit > 200) fit =  200;
            norm_fit = 1. - fit/200.;
            std::dynamic_pointer_cast<are::NN2Individual>(ind)->set_final_position({norm_fit,0,0}); //fake position because mandatory for novelty in this experiment
            ind->setObjectives({norm_fit});
            if(fit < best_fit){
                best_fit = fit;
            }
            std::cout << "Fitness : " << fit << std::endl;
        }
        std::cout << "best fitness : " << best_fit << std::endl;
        nslc.epoch();
        nslc.init_next_pop();
    }

}
