#include "BO.hpp"
#include "ARE/misc/RandNum.h"
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
    parameters.emplace("#initBODataSet",new are_set::Integer(100));
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


    parameters.emplace("#observationType",new are_set::Integer(are_set::obsType::OBJ));
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
    are::misc::RandNum::Ptr rngen (new are::misc::RandNum(seed));

    are::BO bo(rngen,std::make_shared<are_set::ParametersMap>(parameters));
    bo.init();
    bo.set_simulator_side(false);
    int max_iter = 100;
    are::Population pop;
    int gen_size = std::dynamic_pointer_cast<are::NNParamGenome>(bo.get_population()[0]->get_ctrl_genome())->get_full_genome().size();
    std::vector<double> zeros(0,gen_size);
    are::sim::MazeEnv::Ptr env(new are::sim::MazeEnv);
    double best_fit = 1e14;
    std::vector<double> best_gen;
    int eval = 0;
    are::misc::RandNum rn(0);

    while(best_fit > 0.5){
        pop = bo.get_population();
        std::vector<double> genome;
        double fit, norm_fit,avg_fitness = 0;
        double instant_best_fit = 1e14;
        std::cout << "population size " << pop.size() << std::endl;
        for(are::Individual::Ptr& ind : pop){
            genome = std::dynamic_pointer_cast<are::NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
            for(const double& g : genome)
                std::cout << g << " ; ";
            std::cout << std::endl;

            fit = rastrigin(10,genome);
           //fit = sphere(genome);
            avg_fitness+=fit;
            if(fit > 200) fit =  200;
            norm_fit = 1. - fit/200.;
            std::dynamic_pointer_cast<are::BOIndividual>(ind)->set_final_position({norm_fit,0,0}); //fake position because mandatory for novelty in this experiment
            ind->setObjectives({norm_fit});
            if(fit < best_fit){
                best_fit = fit;
            }
            if(fit < instant_best_fit){
                instant_best_fit = fit;
                best_gen = genome;
            }
            bo.update(env);
            eval++;
            std::cout << "Fitness : " << fit << std::endl;
            std::cout << "Norm Fit : " << norm_fit << std::endl;

        }
        std::cout << "best fitness : " << instant_best_fit << " avg fitness : " << avg_fitness/pop.size() << std::endl;
        for(const double& g : best_gen)
                std::cout << g << " ; ";
        std::cout << std::endl;



//        if(!filename.empty()){
//            std::ofstream ofs(filename,std::ofstream::app);
//            ofs << best_fit << std::endl;
//            ofs.close();
//        }

        bo.epoch();
        Eigen::VectorXd mu_gen = std::dynamic_pointer_cast<are::BOLearner>(bo.get_population()[0]->get_learner())->get_mu(best_gen);
        double sigma_gen = std::dynamic_pointer_cast<are::BOLearner>(bo.get_population()[0]->get_learner())->get_sigma(best_gen);
        std::cout << "pred mu converted to objective function scale: " << (1-mu_gen(0))*200 << std::endl;
        std::cout << "uncertainty " << sigma_gen << std::endl;

       // bo.init_next_pop();
        bo.incr_generation();

        std::cout << " =================== " << std::endl;
        std::cout << "GP model test: " << std::endl;

        std::vector<double> randV = rn.randVectd(-5,5,4);
        Eigen::VectorXd v(4);
        v << randV[0], randV[1], randV[2], randV[3];
        Eigen::VectorXd pred_mu = std::dynamic_pointer_cast<are::BOLearner>(bo.get_population()[0]->get_learner())->get_mu(v);
        double sigma = std::dynamic_pointer_cast<are::BOLearner>(bo.get_population()[0]->get_learner())->get_sigma(v);

        std::cout << "test random sample: " << v << std::endl;
        std::cout << "predicted mu: " << pred_mu << std::endl;
        std::cout << "uncertainty: " << sigma << std::endl;
        std::cout << "pred mu converted to objective function scale: " << (1-pred_mu(0))*200 << std::endl;
        std::cout << "objective function " << rastrigin(10,randV) << std::endl;


        std::cout << " =================== " << std::endl;
        std::cout << "Objective: " << std::endl;

        v = Eigen::VectorXd::Zero(4);
        pred_mu = std::dynamic_pointer_cast<are::BOLearner>(bo.get_population()[0]->get_learner())->get_mu(v);
        sigma = std::dynamic_pointer_cast<are::BOLearner>(bo.get_population()[0]->get_learner())->get_sigma(v);

        std::cout << "test random sample: " << v << std::endl;
        std::cout << "predicted mu: " << pred_mu << std::endl;
        std::cout << "uncertainty: " << sigma << std::endl;
        std::cout << "pred mu converted to objective function scale: " << (1-pred_mu(0))*200 << std::endl;
        std::cout << "objective function " << rastrigin(10,{0,0,0,0}) << std::endl;

    }
    std::cout << "Solution found : "  << best_fit << " for target value : "  << rastrigin(10,zeros) << " in " << eval << " evaluations" << std::endl;

}

