#include "mnipes_loggings.hpp"

using namespace are;


void RobotInfoLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    int eval = static_cast<MNIPES*>(ea.get())->get_learners().at(id).nbr_eval();

    std::stringstream sstr;
    sstr << "robot_infos_" << id;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }
    ifs << "evaluation_" << eval << std::endl;
    ifs << "fitnesses;";
    for(double& obj: static_cast<MNIPES*>(ea.get())->get_objectives())
        ifs << obj << ";";
    ifs << std::endl;

    ifs << "trajectory;" << std::endl;
    for(auto& wp: static_cast<MNIPES*>(ea.get())->get_trajectory()){
        ifs << wp.to_string() << std::endl;
    }

    ifs << "ctrl_genome;" << std::endl;
    NNParamGenome gen;
    gen.set_nbr_input(static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_nbr_inputs());
    gen.set_nbr_output(static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_nbr_outputs());
    gen.set_nbr_hidden(settings::getParameter<settings::Integer>(ea->get_parameters(),"#nbrHiddenNeurons").value);
    gen.set_nn_type(settings::getParameter<settings::Integer>(ea->get_parameters(),"#NNType").value);

    std::vector<double> weights, biases;
    for(int i = 0; i < static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_nbr_weights(); i++)
        weights.push_back(static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_population().at(eval-1).genome[i]);
    for(int i = static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_nbr_weights(); i < static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_population().at(eval-1).genome.size(); i++)
        biases.push_back(static_cast<MNIPES*>(ea.get())->get_learners().at(id).get_population().at(eval-1).genome[i]);
    gen.set_weights(weights);
    gen.set_biases(biases);
    ifs << gen.to_string();


    ifs.close();
}


void LearnerStateLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    int eval = static_cast<MNIPES*>(ea.get())->get_learners().at(id).nbr_eval();
    int iter = static_cast<MNIPES*>(ea.get())->get_learners().at(id).nbr_iter();
    std::cout << static_cast<MNIPES*>(ea.get())->get_learners().at(id).print_info() << std::endl;
    std::stringstream sstr;
    sstr << "learner_" << id << "_" << iter << "_" << eval;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }
    ifs << static_cast<MNIPES*>(ea.get())->get_learners().at(id).to_string();
    ifs.close();
}

void BestControllerLog::saveLog(EA::Ptr &ea)
{
    int id = static_cast<MNIPES*>(ea.get())->get_currentID();
    if(!static_cast<MNIPES*>(ea.get())->get_learners().at(id).is_learning_finish())
        return;
    std::stringstream sstr;
    sstr << "/genomes_pool/ctrl_genome_" << id;
    std::ofstream ifs;
    if(!openOLogFile(ifs,sstr.str())){
        std::cerr << "Unable to open file : " << sstr.str() << std::endl;
        return;
    }
    ifs << static_cast<MNIPES*>(ea.get())->get_best_current_ctrl_genome().to_string();
    ifs.close();
}
