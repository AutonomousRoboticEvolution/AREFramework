#include "noEA.h"

using namespace are;
namespace fs = boost::filesystem;

void noEA::init(){
    bool start_from_random = settings::getParameter<settings::Boolean>(parameters,"#startFromRandom").value;
    if(start_from_random){
        int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        rng.Seed(randomNum->getSeed());

        for(int i = 0; i < pop_size; i++){
            EmptyGenome::Ptr morph_gen(new EmptyGenome);
            NNGenome::Ptr ctrl_gen(new NNGenome(randomNum,parameters));
            ctrl_gen->init(rng);
            BOLearner::Ptr learner(new BOLearner);
            learner->set_parameters(parameters);
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

bool noEA::update(const Environment::Ptr & env)
{
    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int nbr_bo_iter = settings::getParameter<settings::Integer>(parameters,"#numberBOIteration").value;

    numberEvaluation++;

    Individual::Ptr ind = population[currentIndIndex];

    //add last fitness and NN weights in the database for the Bayesian optimizer
    Eigen::VectorXd o(3);
    std::vector<double> final_pos = std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position();
    o(0) = final_pos[0];
    o(1) = final_pos[1];
    o(2) = final_pos[2];

    observations.push_back(o);

    auto connections = std::dynamic_pointer_cast<NNControl>(ind->get_control())->nn.m_connections;
    auto neurons = std::dynamic_pointer_cast<NNControl>(ind->get_control())->nn.m_neurons;
    Eigen::VectorXd s(connections.size() + neurons.size());
    int i = 0;
    for(const auto &conn : connections){
        s(i) = conn.m_weight;
        i++;
    }
    for(const auto &neu : neurons){
        s(i) = neu.m_bias;
        i++;
    }

    samples.push_back(s);

    if(verbose)
        std::cout << "Size of dataset for BO : " <<  observations.size() << std::endl;

    if(env->get_name() == "mazeEnv")
        final_position = std::dynamic_pointer_cast<MazeEnv>(env)->get_final_position();
    else if(env->get_name() == "testEnv")
        final_position = std::dynamic_pointer_cast<TestEnv>(env)->get_final_position();

    else std::cerr << "unknown environment" << std::endl;

    if(currentFitnesses.size() == nbr_bo_iter || generation == 0)
    {
        currentFitnesses.clear();
        ind.reset();

        return true;
    }

    if(currentFitnesses.size() == 1){
        std::dynamic_pointer_cast<BOIndividual>(ind)->compute_model(observations,samples);
    }

    std::vector<double> t = std::dynamic_pointer_cast<MazeEnv>(env)->get_target_position();
    Eigen::VectorXd target(3);
    target << t[0], t[1], t[2];

    std::dynamic_pointer_cast<BOIndividual>(ind)->update_learner(observations, samples,target);

    ind.reset();
    return false;
}

void noEA::setObjectives(size_t indIndex, const std::vector<double>& obj){
    currentIndIndex = indIndex;
    currentFitnesses.push_back(obj[0]);
}


void noEA::init_next_pop(){

    int rnd_i = randomNum->randInt(0,population.size());
    BOIndividual::Ptr ind = population[rnd_i];

    population.clear();

    population.push_back(ind);
}


