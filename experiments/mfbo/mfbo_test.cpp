#include "MFBO.hpp"
#include <random>
#include <fstream>

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


int main(int argc, char** argv)
{
    std::string filename;
    if(argc == 3)
        filename = std::string(argv[2]);

    are_set::ParametersMap parameters;
    parameters.emplace("#initBODataSet",new are_set::Integer(10));
    parameters.emplace("#numberBOIteration",new are_set::Integer(1000));
    parameters.emplace("#GPUCBParam", new are_set::Double(std::stod(argv[1])));
    parameters.emplace("#MaxWeight",new are_set::Float(5.));
    parameters.emplace("#verbose",new are_set::Boolean(true));
    parameters.emplace("#kValue",new are_set::Integer(15));
    parameters.emplace("#noveltyThreshold", new are_set::Double(0.9));
    parameters.emplace("#archiveAddingProb", new are_set::Double(0.4));
    parameters.emplace("#NNType",new are_set::Integer(are_set::nnType::FFNN));
    parameters.emplace("#NbrInputNeurones",new are_set::Integer(1));
    parameters.emplace("#NbrHiddenNeurones",new are_set::Integer(0));
    parameters.emplace("#NbrOutputNeurones",new are_set::Integer(1));


    parameters.emplace("#observationType",new are_set::Integer(are_set::obsType::FINAL_POS));
    parameters.emplace("#startFromRandom",new are_set::Boolean(true));
    parameters.emplace("#genToLoad",new are_set::Integer(0));
    parameters.emplace("#genType",new are_set::Integer(0));
    parameters.emplace("#arenaSize",new are_set::Double(2.));

    parameters.emplace("#target_x",new are_set::Double(0));
    parameters.emplace("#target_y",new are_set::Double(0));
    parameters.emplace("#target_z",new are_set::Double(0));
    parameters.emplace("#instanceType",new are_set::Integer(0));

    std::random_device rd;
    int seed = rd();
    misc::RandNum::Ptr rngen (new misc::RandNum(seed));

    are::MFBO mfbo(std::make_shared<are_set::ParametersMap>(parameters));
    mfbo.set_randomNum(rngen);
    mfbo.init();
    int max_iter = 100;
    std::vector<are::Individual::Ptr> pop;
    int gen_size = std::dynamic_pointer_cast<are::NNParamGenome>(mfbo.get_population()[0]->get_ctrl_genome())->get_full_genome().size();
    std::vector<double> zeros(0,gen_size);
    are::MazeEnv::Ptr env(new are::MazeEnv);
    double best_fit = 1e14;
    std::vector<double> best_gen;
    int eval = 0;
    while(best_fit > 0.5){
        pop = mfbo.get_population();
        std::vector<double> genome;
        double fit, norm_fit;
        for(are::Individual::Ptr& ind : pop){
            genome = std::dynamic_pointer_cast<are::NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
            for(const double& g : genome)
                std::cout << g << " ; ";
            std::cout << std::endl;

            fit = rastrigin(10,genome);
            //fit = sphere(genome);
            if(fit > 200) fit =  200;
            norm_fit = 1. - fit/200.;
            std::dynamic_pointer_cast<are::BOIndividual>(ind)->set_final_position({norm_fit,0,0}); //fake position because mandatory for novelty in this experiment
            ind->setObjectives({norm_fit});
            if(fit < best_fit){
                best_fit = fit;
                best_gen = genome;
            }
            mfbo.update(env);
            eval++;
            std::cout << "Fitness : " << fit << std::endl;
        }
        std::cout << "best fitness : " << best_fit << std::endl;
        for(const double& g : best_gen)
                std::cout << g << " ; ";
        std::cout << std::endl;
        if(!filename.empty()){
            std::ofstream ofs(filename,std::ofstream::app);
            ofs << best_fit << std::endl;
            ofs.close();
        }
        mfbo.epoch();
        mfbo.init_next_pop();
    }
    std::cout << "Solution found : "  << best_fit << " for target value : "  << rastrigin(10,zeros) << " in " << eval << " evaluations" << std::endl;

}
