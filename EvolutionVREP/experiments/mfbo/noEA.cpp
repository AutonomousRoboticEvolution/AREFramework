#include "noEA.h"

using namespace are;
namespace fs = boost::filesystem;

void noEA::init(){
    bool start_from_random = settings::getParameter<settings::Boolean>(parameters,"#startFromRandom").value;
    if(start_from_random){
        int init_bo_dataset = settings::getParameter<settings::Integer>(parameters,"#initBODataSet").value;
        float max_weight = settings::getParameter<settings::Float>(parameters,"#MaxWeight").value;

        NNGenome nn_gen(randomNum,parameters);
        NEAT::NeuralNetwork nn;
        nn_gen.init();
        nn_gen.buildPhenotype(nn);
        int nbr_weights = nn.m_connections.size();
        int nbr_biases = nn.m_neurons.size();

        Eigen::MatrixXd init_samples = limbo::tools::random_lhs(nbr_weights + nbr_biases,init_bo_dataset);

        std::vector<double> weights(nbr_weights);
        std::vector<double> biases(nbr_biases);

        for(int i = 0; i < init_bo_dataset; i++){
            for(int v = 0; v < nbr_weights; v++)
                weights[v] = max_weight*(init_samples(i,v)*2.f - 1.f);
            for(int w = nbr_weights; w < nbr_weights+nbr_biases; w++)
                biases[w-nbr_weights] = max_weight*(init_samples(i,w)*2.f - 1.f);

            EmptyGenome::Ptr morph_gen(new EmptyGenome);
            NNParamGenome::Ptr ctrl_gen(new NNParamGenome);
            ctrl_gen->set_weights(weights);
            ctrl_gen->set_biases(biases);
            BOLearner::Ptr learner(new BOLearner(parameters));
            Individual::Ptr ind(new BOIndividual(morph_gen,ctrl_gen,learner));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }
        return;
    }

    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    int generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
    int genType = settings::getParameter<settings::Integer>(parameters,"#genType").value;
    std::string filename;
    std::vector<std::string> split_str, gen_files;
    for(const auto &dirit : fs::directory_iterator(fs::path(folder_to_load))){
        filename = dirit.path().string();
        boost::split(split_str,filename,boost::is_any_of("/"));
        boost::split(split_str,split_str.back(),boost::is_any_of("_"));
        if(split_str[0] == "genome" &&
                std::stoi(split_str[1]) == generation){
            gen_files.push_back(filename);
        }
    }

    for(const std::string& file: gen_files){
        EmptyGenome::Ptr morph_gen(new EmptyGenome);
        Genome::Ptr genome;

        if(genType == settings::NEAT){
            //todo
        }
        else if(genType == settings::NN){
            genome.reset(new NNGenome(randomNum,parameters));
            NEAT::Genome neat_genome(file.c_str());
            std::dynamic_pointer_cast<NNGenome>(genome)->set_nn_genome(neat_genome);
        }else if(genType == settings::NNPARAM){
            genome.reset(new NNParamGenome(randomNum,parameters));
            NNParamGenome::Ptr nngenome = std::dynamic_pointer_cast<NNParamGenome>(genome);
            std::ifstream logFileStream;
            logFileStream.open(file);
            std::string line;
            std::getline(logFileStream,line);
            int nbr_weights = std::stoi(line);
            std::getline(logFileStream,line);
            int nbr_bias = std::stoi(line);

            std::vector<double> weights;
            for(int i = 0; i < nbr_weights; i++){
                std::getline(logFileStream,line);
                weights.push_back(std::stod(line));
            }
            nngenome->set_weights(weights);


            std::vector<double> biases;
            for(int i = 0; i < nbr_bias; i++){
                std::getline(logFileStream,line);
                biases.push_back(std::stod(line));
            }
            nngenome->set_biases(biases);


        }
        BOLearner::Ptr learner(new BOLearner);
        learner->set_parameters(parameters);
        Individual::Ptr ind(new BOIndividual(morph_gen,genome,learner));
        ind->set_parameters(parameters);
        std::dynamic_pointer_cast<BOIndividual>(ind)->genType = genType;
        population.push_back(ind);
    }

}

void noEA::epoch(){

    if(generation == 0){
        int rnd_i = randomNum->randInt(0,population.size()-1);
        BOIndividual::Ptr ind = population[rnd_i];
        population.clear();
        population.push_back(ind);
        currentIndIndex = 0;
    }

    Individual::Ptr ind = population[currentIndIndex];
    if(currentFitnesses.size() == 1){
        std::dynamic_pointer_cast<BOIndividual>(ind)->compute_model(observations,samples);
    }

    Eigen::VectorXd target(3);
    target << settings::getParameter<settings::Double>(parameters,"#target_x").value,
            settings::getParameter<settings::Double>(parameters,"#target_y").value,
            settings::getParameter<settings::Double>(parameters,"#target_z").value;

    std::dynamic_pointer_cast<BOIndividual>(ind)->update_learner(observations, samples,target);
    ind.reset();
}

bool noEA::update(const Environment::Ptr & env)
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int instance_type = settings::getParameter<settings::Integer>(parameters,"#instanceType").value;

    numberEvaluation++;
    Individual::Ptr ind = population[currentIndIndex];

    if(simulator_side)
        std::dynamic_pointer_cast<BOIndividual>(ind)->set_final_position(
                    std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position());

    if(instance_type == settings::INSTANCE_REGULAR || !simulator_side){

        //add last fitness and NN weights in the database for the Bayesian optimizer
        Eigen::VectorXd o(3);
        std::vector<double> final_pos = std::dynamic_pointer_cast<BOIndividual>(ind)->get_final_position();
        o(0) = final_pos[0];
        o(1) = final_pos[1];
        o(2) = final_pos[2];

        observations.push_back(o);

        std::vector<double> nn_params = std::dynamic_pointer_cast<NNParamGenome>(ind->get_ctrl_genome())->get_full_genome();
        Eigen::VectorXd s(nn_params.size());
        for(size_t i = 0; i < nn_params.size(); i++)
            s(i) = nn_params[i];

        samples.push_back(s);

        if(verbose)
            std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;

        ind.reset();
    }
    return true;
}

void noEA::setObjectives(size_t indIndex, const std::vector<double>& obj){
    currentIndIndex = indIndex;
    population[currentIndIndex]->setObjectives(obj);
    currentFitnesses.push_back(obj[0]);
    double ftarget = settings::getParameter<settings::Double>(parameters,"#FTarget").value;
    _is_finish = obj[0] <= ftarget;
}

bool noEA::is_finish(){
    int init_bo_dataset = settings::getParameter<settings::Integer>(parameters,"#initBODataSet").value;
    int nbr_bo_iter = settings::getParameter<settings::Integer>(parameters,"#numberBOIteration").value;
    return _is_finish || numberEvaluation >= nbr_bo_iter + init_bo_dataset;
}
