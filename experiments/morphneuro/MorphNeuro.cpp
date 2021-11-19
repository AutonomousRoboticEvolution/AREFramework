#include "MorphNeuro.h"
#include <iostream>
#include <fstream>
#include <string>
#include "mazeEnv_multiTarget.h"
#include "v_repLib.h"

using namespace are;

void MorphNeuro::init()
{
    ////Initiate parameters
    params = NEAT::Parameters();
    //// Set parameters for NEAT
    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    params.EliteFraction = 0.05;
    params.PopulationSize = pop_size;
    params.DynamicCompatibility = true;
    params.CompatTreshold = 2.0;
    params.YoungAgeTreshold = 15;
    params.SpeciesMaxStagnation = 100;
    params.OldAgeTreshold = 50;
    params.MinSpecies = 5;
    params.MaxSpecies = 10;
    params.RouletteWheelSelection = false;

    params.MutateRemLinkProb = 0.02;
    params.RecurrentProb = 0;
    params.OverallMutationRate = 0.15;
    params.MutateAddLinkProb = 0.08;
    params.MutateAddNeuronProb = 0.01;
    params.MutateWeightsProb = 0.90;
    params.MaxWeight = 8.0;
    params.WeightMutationMaxPower = 0.2;
    params.WeightReplacementMaxPower = 1.0;

    params.MutateActivationAProb = 0.0;
    params.ActivationAMutationMaxPower = 0.5;
    params.MinActivationA = 0.05;
    params.MaxActivationA = 6.0;

    params.MutateNeuronActivationTypeProb = 0.03;

    // Crossover
    params.SurvivalRate = 0.01;
    params.CrossoverRate = 0.01;
    params.InterspeciesCrossoverRate = 0.01;

    params.ActivationFunction_SignedSigmoid_Prob = 0.0;
    params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
    params.ActivationFunction_Tanh_Prob = 1.0;
    params.ActivationFunction_TanhCubic_Prob = 0.0;
    params.ActivationFunction_SignedStep_Prob = 1.0;
    params.ActivationFunction_UnsignedStep_Prob = 0.0;
    params.ActivationFunction_SignedGauss_Prob = 1.0;
    params.ActivationFunction_UnsignedGauss_Prob = 0.0;
    params.ActivationFunction_Abs_Prob = 0.0;
    params.ActivationFunction_SignedSine_Prob = 1.0;
    params.ActivationFunction_UnsignedSine_Prob = 0.0;
    params.ActivationFunction_Linear_Prob = 1.0;

    /////************************

    paramsCtrl = NEAT::Parameters();
//    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    paramsCtrl.EliteFraction = 0.05;
    paramsCtrl.PopulationSize = pop_size;
    paramsCtrl.DynamicCompatibility = true;
    paramsCtrl.CompatTreshold = 2.0;
    paramsCtrl.YoungAgeTreshold = 15;
    paramsCtrl.SpeciesMaxStagnation = 100;
    paramsCtrl.OldAgeTreshold = 50;
    paramsCtrl.MinSpecies = 5;
    paramsCtrl.MaxSpecies = 10;
    paramsCtrl.RouletteWheelSelection = false;

    paramsCtrl.MutateRemLinkProb = 0.02;
    bool isRNN = settings::getParameter<settings::Boolean>(parameters,"#isRNN").value;
    if(isRNN) {
        paramsCtrl.RecurrentProb = 1;
    }else{
        paramsCtrl.RecurrentProb = 0;
    }
    paramsCtrl.OverallMutationRate = 0.15;
    paramsCtrl.MutateAddLinkProb = 0.08;
    paramsCtrl.MutateAddNeuronProb = 0.01;
    paramsCtrl.MutateWeightsProb = 0.90;
    paramsCtrl.MaxWeight = 8.0;
    paramsCtrl.WeightMutationMaxPower = 0.2;
    paramsCtrl.WeightReplacementMaxPower = 1.0;

    paramsCtrl.MutateActivationAProb = 0.0;
    paramsCtrl.ActivationAMutationMaxPower = 0.5;
    paramsCtrl.MinActivationA = 0.05;
    paramsCtrl.MaxActivationA = 6.0;

    paramsCtrl.MutateNeuronActivationTypeProb = 0.03;

    // Crossover
    paramsCtrl.SurvivalRate = 0.01;
    paramsCtrl.CrossoverRate = 0.01;
    paramsCtrl.InterspeciesCrossoverRate = 0.01;

    paramsCtrl.ActivationFunction_SignedSigmoid_Prob = 0.0;
    paramsCtrl.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
    paramsCtrl.ActivationFunction_Tanh_Prob = 1.0;
    paramsCtrl.ActivationFunction_TanhCubic_Prob = 0.0;
    paramsCtrl.ActivationFunction_SignedStep_Prob = 1.0;
    paramsCtrl.ActivationFunction_UnsignedStep_Prob = 0.0;
    paramsCtrl.ActivationFunction_SignedGauss_Prob = 1.0;
    paramsCtrl.ActivationFunction_UnsignedGauss_Prob = 0.0;
    paramsCtrl.ActivationFunction_Abs_Prob = 0.0;
    paramsCtrl.ActivationFunction_SignedSine_Prob = 1.0;
    paramsCtrl.ActivationFunction_UnsignedSine_Prob = 0.0;
    paramsCtrl.ActivationFunction_Linear_Prob = 1.0;

    initPopulation();

    Novelty::archive_adding_prob = settings::getParameter<settings::Double>(parameters,"#archiveAddingProbability").value;
    Novelty::novelty_thr = settings::getParameter<settings::Double>(parameters,"#noveltyThreshold").value;

}

void MorphNeuro::initPopulation()
{
    rng.Seed(randomNum->getSeed());
    //// Initiate genomes
    NEAT::Genome morph_genome(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, params, 10);
    NEAT::Genome contr_genome(0, 7, 10, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, paramsCtrl, 2);
    randomNum->setSeed(time(0));
    int population_rand_int = randomNum->randInt(0, 10000);
    morph_population = std::make_unique<NEAT::Population>(morph_genome, params, true, 1.0, population_rand_int);
    contr_population = std::make_unique<NEAT::Population>(contr_genome, paramsCtrl, true, 1.0, population_rand_int);

    int manufacturabilityMethod = settings::getParameter<settings::Integer>(parameters,"#manufacturabilityMethod").value;
    //// Morphology bootstrap
    bool isBootstrapPopulation = settings::getParameter<settings::Boolean>(parameters,"#isBootstrapEvolution").value;
    std::vector<int> robotList;
    if(isBootstrapPopulation) {
        robotList = listInds();
    }
    //// Controller bootstrap - work in progess, may or may not needed
    //// Method 1: Import controller genome files
    //// Method 2: 1-many method for morphology
    bool iscontrlbootstrap_1 = settings::getParameter<settings::Boolean>(parameters,"#isCtrlBootstrapEvolution_1").value;
    bool iscontrlbootstrap_2 = settings::getParameter<settings::Boolean>(parameters,"#isCtrlBootstrapEvolution_2").value;
    for (size_t i = 0; i < params.PopulationSize ; i++)
    {
//        if(isBootstrapPopulation) {
//            NEAT::Genome indGenome;
//            indGenome = loadInd(robotList[i]);
//            morph_population->AccessGenomeByIndex(i) = indGenome;  //individual genome
//        }
//
//        if(iscontrlbootstrap_2){
////      controller bootstrap method 2
//            NEAT::Genome indGenome_contr;
//            indGenome_contr = loadContrInd(robotList[i]);
//            contr_population->AccessGenomeByIndex(i) = indGenome_contr;  //individual genome
//
//        }
        // Creat initial population
        CPPNGenome::Ptr contrlGenome(new CPPNGenome(contr_population->AccessGenomeByIndex(i)));
        CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
//        EmptyGenome::Ptr no_gen(new EmptyGenome);
//        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,no_gen));
        CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,contrlGenome));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);

    }
}

void MorphNeuro::epoch() {
    ////Core of EA framework
    ////Epoch does the process of handling genomes, including seleciton, mutation, etc.

    ////log fitness for all individuals from population
    int idv_count = 0;
    std::vector<double> fitness_log_all;
    fitness_log_all.empty();
    for (const auto &ind : population) {
        idv_count++;
        fitness_log_all.push_back(ind->getObjectives()[0]);
    }
    int pop_size = settings::getParameter<settings::Integer>(parameters, "#populationSize").value;
    double best_fitness = 0.0;
    int best_idx = 0;
    std::vector<double> idx_to_keep;
    idx_to_keep.empty();

    //// 1-many, dual loop, sigle loop parameters
    bool is1tomany = settings::getParameter<settings::Boolean>(parameters, "#is1tomany").value;
    bool isdualoop = settings::getParameter<settings::Boolean>(parameters, "#isDuaLoop").value;
    bool issingleloop = settings::getParameter<settings::Boolean>(parameters, "#issingleloop").value;

    //// 1-many method
    if (is1tomany && idv_count != pop_size) {
        //// selection individuals from 1-many population
        int eval_num = settings::getParameter<settings::Integer>(parameters, "#eval_num").value;
        int o_to_m_counter = 0;
        for (int i = 0; i <= idv_count; i++) {
            if (o_to_m_counter != 0 && o_to_m_counter % eval_num == 0) {
                idx_to_keep.push_back(best_idx);
                best_fitness = fitness_log_all[i];
                best_idx = i;
                o_to_m_counter = 1;
            } else {
                double current_fitness = fitness_log_all[i];
                if (current_fitness > best_fitness) {
                    best_fitness = current_fitness;
                    best_idx = i;
                }
                o_to_m_counter += 1;
            }
        }
        std::cout << "idx_to_keep " << idx_to_keep.size() << std::endl;
        std::vector<Individual::Ptr> temp_population;
        for (int i = 0; i < pop_size; i++) {
            temp_population.push_back(population[idx_to_keep[i]]);
        }
        population = temp_population;

        //// Novelty for selected population
        bool isNovelty = settings::getParameter<settings::Boolean>(parameters, "#isNovelty").value;
        if (isNovelty) {
            // Novelty
            double linearCombAlpha = settings::getParameter<settings::Double>(parameters, "#linearCombAlpha").value;
            if (settings::getParameter<settings::Double>(parameters, "#noveltyRatio").value > 0.) {
                if (Novelty::k_value >= pop_size)
                    Novelty::k_value = pop_size / 2;
                else Novelty::k_value = settings::getParameter<settings::Integer>(parameters, "#kValue").value;


                std::vector<Eigen::VectorXd> pop_desc;
                for (const auto &ind : population) {
                    std::vector<double> final_position = std::dynamic_pointer_cast<CPPNIndividual>(
                            ind)->get_final_position();
                    pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(ind)->descriptor(final_position));
                }

                //compute novelty score
                std::vector<std::vector<double>> new_obj;
                for (const auto &ind : population) {
                    std::vector<double> final_position = std::dynamic_pointer_cast<CPPNIndividual>(
                            ind)->get_final_position();
                    Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->descriptor(
                            final_position);

                    //Compute distances to find the k nearest neighbors of ind
                    std::vector<size_t> pop_indexes;
                    std::vector<double> distances = Novelty::distances(ind_desc, archive, pop_desc, pop_indexes);

                    //Compute novelty
                    double ind_nov = Novelty::sparseness(distances);

                    /** Linear Combination **/
                    double fitScore = ind->getObjectives()[0];//std::dynamic_pointer_cast<CPPNIndividual>(ind)->getManScore();

                    //set the objetives
                    std::vector<double> objectives = {linearCombAlpha * (ind_nov / 2.64) + (1 - linearCombAlpha) *
                                                                                           fitScore}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
                    std::dynamic_pointer_cast<CPPNIndividual>(ind)->addObjective(objectives.back());
                    new_obj.push_back(objectives);
                }

                //update archive for novelty score
                int pop_count = 0;
                for (const auto &ind : population) {
                    Eigen::VectorXd ind_desc;
                    std::vector<double> final_position = std::dynamic_pointer_cast<CPPNIndividual>(
                            ind)->get_final_position();
                    ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->descriptor(final_position);

                    double ind_nov = new_obj[pop_count].back();//ind->getObjectives().back();
                    pop_count += 1;
                    Novelty::update_archive(ind_desc, ind_nov, archive, randomNum);
                }
            }
        }

        //// Epoch for morphology and controller, mutation, selection and reproduction
        int indCounter = 0;
        for (const auto &ind : population) {
            morph_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
            contr_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
            indCounter++;
        }

        morph_population->Epoch();
        contr_population->Epoch();
    }

    if (isdualoop && idv_count != pop_size) {
        //// dual loop method
        static int loop_counter = 0;
        // initiate parameters for inner loop
        int eval_num = settings::getParameter<settings::Integer>(parameters, "#eval_num").value;
        int inner_loop_number = settings::getParameter<settings::Integer>(parameters, "#inner_loop_num").value;
        //// initiate learning loop population parameters
        inner_params = NEAT::Parameters();
        /// Set parameters for NEAT
        inner_params.EliteFraction = 0.05;
        inner_params.PopulationSize = eval_num;
        inner_params.DynamicCompatibility = true;
        inner_params.CompatTreshold = 2.0;
        inner_params.YoungAgeTreshold = 15;
        inner_params.SpeciesMaxStagnation = 100;
        inner_params.OldAgeTreshold = 50;
        inner_params.MinSpecies = 5;
        inner_params.MaxSpecies = 10;
        inner_params.RouletteWheelSelection = false;

        inner_params.MutateRemLinkProb = 0.02;
        inner_params.RecurrentProb = 0;
        inner_params.OverallMutationRate = 0.15;
        inner_params.MutateAddLinkProb = 0.08;
        inner_params.MutateAddNeuronProb = 0.01;
        inner_params.MutateWeightsProb = 0.90;
        inner_params.MaxWeight = 8.0;
        inner_params.WeightMutationMaxPower = 0.2;
        inner_params.WeightReplacementMaxPower = 1.0;

        inner_params.MutateActivationAProb = 0.0;
        inner_params.ActivationAMutationMaxPower = 0.5;
        inner_params.MinActivationA = 0.05;
        inner_params.MaxActivationA = 6.0;

        inner_params.MutateNeuronActivationTypeProb = 0.03;

        // Crossover
        inner_params.SurvivalRate = 0.01;
        inner_params.CrossoverRate = 0.01;
        inner_params.InterspeciesCrossoverRate = 0.01;

        inner_params.ActivationFunction_SignedSigmoid_Prob = 0.0;
        inner_params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
        inner_params.ActivationFunction_Tanh_Prob = 1.0;
        inner_params.ActivationFunction_TanhCubic_Prob = 0.0;
        inner_params.ActivationFunction_SignedStep_Prob = 1.0;
        inner_params.ActivationFunction_UnsignedStep_Prob = 0.0;
        inner_params.ActivationFunction_SignedGauss_Prob = 1.0;
        inner_params.ActivationFunction_UnsignedGauss_Prob = 0.0;
        inner_params.ActivationFunction_Abs_Prob = 0.0;
        inner_params.ActivationFunction_SignedSine_Prob = 1.0;
        inner_params.ActivationFunction_UnsignedSine_Prob = 0.0;
        inner_params.ActivationFunction_Linear_Prob = 1.0;

        /////************************

        inner_paramsCtrl = NEAT::Parameters();
//    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
        inner_paramsCtrl.EliteFraction = 0.05;
        inner_paramsCtrl.PopulationSize = eval_num;
        inner_paramsCtrl.DynamicCompatibility = true;
        inner_paramsCtrl.CompatTreshold = 2.0;
        inner_paramsCtrl.YoungAgeTreshold = 15;
        inner_paramsCtrl.SpeciesMaxStagnation = 100;
        inner_paramsCtrl.OldAgeTreshold = 50;
        inner_paramsCtrl.MinSpecies = 5;
        inner_paramsCtrl.MaxSpecies = 10;
        inner_paramsCtrl.RouletteWheelSelection = false;

        inner_paramsCtrl.MutateRemLinkProb = 0.02;
        bool isRNN = settings::getParameter<settings::Boolean>(parameters, "#isRNN").value;
        if (isRNN) {
            inner_paramsCtrl.RecurrentProb = 1;
        } else {
            inner_paramsCtrl.RecurrentProb = 0;
        }
        inner_paramsCtrl.OverallMutationRate = 0.15;
        inner_paramsCtrl.MutateAddLinkProb = 0.08;
        inner_paramsCtrl.MutateAddNeuronProb = 0.01;
        inner_paramsCtrl.MutateWeightsProb = 0.90;
        inner_paramsCtrl.MaxWeight = 8.0;
        inner_paramsCtrl.WeightMutationMaxPower = 0.2;
        inner_paramsCtrl.WeightReplacementMaxPower = 1.0;

        inner_paramsCtrl.MutateActivationAProb = 0.0;
        inner_paramsCtrl.ActivationAMutationMaxPower = 0.5;
        inner_paramsCtrl.MinActivationA = 0.05;
        inner_paramsCtrl.MaxActivationA = 6.0;

        inner_paramsCtrl.MutateNeuronActivationTypeProb = 0.03;

        // Crossover
        inner_paramsCtrl.SurvivalRate = 0.01;
        inner_paramsCtrl.CrossoverRate = 0.01;
        inner_paramsCtrl.InterspeciesCrossoverRate = 0.01;

        inner_paramsCtrl.ActivationFunction_SignedSigmoid_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_Tanh_Prob = 1.0;
        inner_paramsCtrl.ActivationFunction_TanhCubic_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_SignedStep_Prob = 1.0;
        inner_paramsCtrl.ActivationFunction_UnsignedStep_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_SignedGauss_Prob = 1.0;
        inner_paramsCtrl.ActivationFunction_UnsignedGauss_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_Abs_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_SignedSine_Prob = 1.0;
        inner_paramsCtrl.ActivationFunction_UnsignedSine_Prob = 0.0;
        inner_paramsCtrl.ActivationFunction_Linear_Prob = 1.0;

        //// initiate inner loop population
        NEAT::Genome contr_genome(0, 7, 10, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, inner_paramsCtrl,
                                  2);
        int inner_population_rand_int = randomNum->randInt(0, 10000);

        inner_contr_population = std::make_unique<NEAT::Population>(contr_genome, inner_paramsCtrl, true, 1.0,
                                                                    inner_population_rand_int);
        //// conduct Epoch ONLY on controllers
        if (loop_counter < inner_loop_number){//while in inner loop
            std::cout << "Inside Inner Loop" << loop_counter <<std::endl;
            int o_to_m_counter = 0;
            std::vector<Individual::Ptr> inner_population(eval_num);
            /*multi-eva*/ ////NEED TO BE MODIFIED
            for (int i = 0; i < idv_count; i++) {
                inner_population[o_to_m_counter] = population[i];
                inner_contr_population ->AccessGenomeByIndex(o_to_m_counter) = multi_contr_population->AccessGenomeByIndex(i);
                o_to_m_counter++;
                if (o_to_m_counter != 0 && o_to_m_counter % eval_num == 0) {
                    int indCounter = 0;
                    for (const auto &ind : inner_population) {
                        std::cout << indCounter <<std::endl;
                        inner_contr_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
                        indCounter++;
                    }
                    inner_contr_population->Epoch();
                    for (int j = 0; j < eval_num; j++) {
                        multi_contr_population->AccessGenomeByIndex(int(i +1 - eval_num + j)) = inner_contr_population->AccessGenomeByIndex(j);
                    }
                    o_to_m_counter = 0;
                }
            }
            loop_counter++;
        }else{
            //// For evolution loop, conduct Epoch on both morphology and controller
            std::cout << "Inside Outer Loop" << std::endl;
            loop_counter = 0;
            //do 1-many selection and epoch both morphology and controller
            int o_to_m_counter = 0;
            /*multi-eva*/ ////NEED TO BE MODIFIED
            for (int i = 0; i <= idv_count; i++) {
                if (o_to_m_counter != 0 && o_to_m_counter % eval_num == 0) {
                    idx_to_keep.push_back(best_idx);
                    best_fitness = fitness_log_all[i];
                    best_idx = i;
                    o_to_m_counter = 1;
                } else {
                    double current_fitness = fitness_log_all[i];
                    if (current_fitness > best_fitness) {
                        best_fitness = current_fitness;
                        best_idx = i;
                    }
                    o_to_m_counter += 1;
                }
            }
            std::cout << "idx_to_keep " << idx_to_keep.size() << std::endl;
            std::vector<Individual::Ptr> temp_population;

            //// Lamarckian method, inheret inner loop controller
            bool lamarkian = settings::getParameter<settings::Boolean>(parameters, "#islamarkian").value;
            for (int i = 0; i < pop_size; i++) {
                temp_population.push_back(population[idx_to_keep[i]]);
                if(lamarkian){
                    contr_population->AccessGenomeByIndex(i) = multi_contr_population->AccessGenomeByIndex(idx_to_keep[i]);
                }

            }

            population = temp_population;


            //// Novelty method
            bool isNovelty = settings::getParameter<settings::Boolean>(parameters, "#isNovelty").value;
            if (isNovelty) {
                // Novelty
                double linearCombAlpha = settings::getParameter<settings::Double>(parameters, "#linearCombAlpha").value;
                if (settings::getParameter<settings::Double>(parameters, "#noveltyRatio").value > 0.) {
                    if (Novelty::k_value >= pop_size)
                        Novelty::k_value = pop_size / 2;
                    else Novelty::k_value = settings::getParameter<settings::Integer>(parameters, "#kValue").value;


                    std::vector<Eigen::VectorXd> pop_desc;
                    for (const auto &ind : population) {
                        std::vector<double> final_position = std::dynamic_pointer_cast<CPPNIndividual>(
                                ind)->get_final_position();
                        pop_desc.push_back(std::dynamic_pointer_cast<CPPNIndividual>(ind)->descriptor(final_position));
                    }

                    //compute novelty score
                    std::vector<std::vector<double>> new_obj;
                    for (const auto &ind : population) {
                        std::vector<double> final_position = std::dynamic_pointer_cast<CPPNIndividual>(
                                ind)->get_final_position();
                        Eigen::VectorXd ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->descriptor(
                                final_position);

                        //Compute distances to find the k nearest neighbors of ind
                        std::vector<size_t> pop_indexes;
                        std::vector<double> distances = Novelty::distances(ind_desc, archive, pop_desc, pop_indexes);

                        //Compute novelty
                        double ind_nov = Novelty::sparseness(distances);

                        /** Linear Combination **/
                        double fitScore = ind->getObjectives()[0];//std::dynamic_pointer_cast<CPPNIndividual>(ind)->getManScore();

                        //set the objetives
                        std::vector<double> objectives = {linearCombAlpha * (ind_nov / 2.64) + (1 - linearCombAlpha) *
                                                                                               fitScore}; /// \todo EB: define 2.64 as constant. This constants applies only for cartesian descriptor!
                        std::dynamic_pointer_cast<CPPNIndividual>(ind)->addObjective(objectives.back());
                        new_obj.push_back(objectives);
                    }

                    //update archive for novelty score
                    int pop_count = 0;
                    for (const auto &ind : population) {
                        Eigen::VectorXd ind_desc;
                        std::vector<double> final_position = std::dynamic_pointer_cast<CPPNIndividual>(
                                ind)->get_final_position();
                        ind_desc = std::dynamic_pointer_cast<CPPNIndividual>(ind)->descriptor(final_position);

                        double ind_nov = new_obj[pop_count].back();//ind->getObjectives().back();
                        pop_count += 1;
                        Novelty::update_archive(ind_desc, ind_nov, archive, randomNum);
                    }
                }
            }

            //// Conduct Epoch on both on morphology and controller genome, notice that controller genome is different for case with/ without Lamarckian method
            int indCounter = 0;
            for (const auto &ind : population) {
                morph_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
                contr_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
                indCounter++;
            }

            morph_population->Epoch();
            contr_population->Epoch();
        }

    }

    if (issingleloop){
        //// Single loop, Epoch is conducted on both morphology and controller
        int indCounter = 0; /// \todo EB: There must be a better way to do this!
        for(const auto& ind : population){
            morph_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
            // TO CHANGE
            contr_population->AccessGenomeByIndex(indCounter).SetFitness(ind->getObjectives().back());
            indCounter++;
        }
        morph_population->Epoch();
        // TO CHANGE
        contr_population->Epoch();

    }
}


void MorphNeuro::setObjectives(size_t indIdx, const std::vector<double> &  objectives){
    //// setObjectives is to set fitness to individual
    currentIndIndex = indIdx;
    population[indIdx]->setObjectives(objectives);
}

void MorphNeuro::init_next_pop(){
    //// prepare population for next generation of evolution
    int manufacturabilityMethod = settings::getParameter<settings::Integer>(parameters,"#manufacturabilityMethod").value;
    int pop_size_current = 0;
    for (const auto &ind : population) {
        pop_size_current++;
    }
    population.clear();
    //// differet methods require different initial popualtion
    //// notice that for dual loop method, simulation/src/mazeEnv.cpp needs to be modified
    int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
    bool ismultieval = settings::getParameter<settings::Boolean>(parameters,"#isMulti").value;
    bool ismultieval_random = settings::getParameter<settings::Boolean>(parameters,"#isMultiRandom").value;
    bool ismultieval_mutate = settings::getParameter<settings::Boolean>(parameters,"#isMultiMutate").value;
    int eval_num = settings::getParameter<settings::Integer>(parameters,"#eval_num").value;
    int inner_loop_number = settings::getParameter<settings::Integer>(parameters, "#inner_loop_num").value;
    for (int i = 0; i < pop_size; i++)
    {
        if (ismultieval && pop_size_current == pop_size){
            //// in the case of 1-many method and inner learning loop of dual loop method, population size depends on the setting
            //// the first generation of inner learning loop is generate
            multi_params = NEAT::Parameters();
            /// Set parameters for NEAT
            multi_params.EliteFraction = 0.2;
            multi_params.PopulationSize = int(eval_num * pop_size);
            multi_params.DynamicCompatibility = true;
            multi_params.CompatTreshold = 2.0;
            multi_params.YoungAgeTreshold = 15;
            multi_params.SpeciesMaxStagnation = 100;
            multi_params.OldAgeTreshold = 50;
            multi_params.MinSpecies = 5;
            multi_params.MaxSpecies = 10;
            multi_params.RouletteWheelSelection = false;

            multi_params.MutateRemLinkProb = 0.02;
            multi_params.RecurrentProb = 0;
            multi_params.OverallMutationRate = 0.15;
            multi_params.MutateAddLinkProb = 0.08;
            multi_params.MutateAddNeuronProb = 0.01;
            multi_params.MutateWeightsProb = 0.90;
            multi_params.MaxWeight = 8.0;
            multi_params.WeightMutationMaxPower = 0.2;
            multi_params.WeightReplacementMaxPower = 1.0;

            multi_params.MutateActivationAProb = 0.0;
            multi_params.ActivationAMutationMaxPower = 0.5;
            multi_params.MinActivationA = 0.05;
            multi_params.MaxActivationA = 6.0;

            multi_params.MutateNeuronActivationTypeProb = 0.03;

            // Crossover
            multi_params.SurvivalRate = 0.01;
            multi_params.CrossoverRate = 0.01;
            multi_params.InterspeciesCrossoverRate = 0.01;

            multi_params.ActivationFunction_SignedSigmoid_Prob = 0.0;
            multi_params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
            multi_params.ActivationFunction_Tanh_Prob = 1.0;
            multi_params.ActivationFunction_TanhCubic_Prob = 0.0;
            multi_params.ActivationFunction_SignedStep_Prob = 1.0;
            multi_params.ActivationFunction_UnsignedStep_Prob = 0.0;
            multi_params.ActivationFunction_SignedGauss_Prob = 1.0;
            multi_params.ActivationFunction_UnsignedGauss_Prob = 0.0;
            multi_params.ActivationFunction_Abs_Prob = 0.0;
            multi_params.ActivationFunction_SignedSine_Prob = 1.0;
            multi_params.ActivationFunction_UnsignedSine_Prob = 0.0;
            multi_params.ActivationFunction_Linear_Prob = 1.0;

            /////************************

            multi_paramsCtrl = NEAT::Parameters();
//    unsigned int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
            multi_paramsCtrl.EliteFraction = 0.2;
            multi_paramsCtrl.PopulationSize = int(eval_num * pop_size);
            multi_paramsCtrl.DynamicCompatibility = true;
            multi_paramsCtrl.CompatTreshold = 2.0;
            multi_paramsCtrl.YoungAgeTreshold = 15;
            multi_paramsCtrl.SpeciesMaxStagnation = 100;
            multi_paramsCtrl.OldAgeTreshold = 50;
            multi_paramsCtrl.MinSpecies = 5;
            multi_paramsCtrl.MaxSpecies = 10;
            multi_paramsCtrl.RouletteWheelSelection = false;

            multi_paramsCtrl.MutateRemLinkProb = 0.02;
            bool isRNN = settings::getParameter<settings::Boolean>(parameters, "#isRNN").value;
            if (isRNN) {
                multi_paramsCtrl.RecurrentProb = 1;
            } else {
                multi_paramsCtrl.RecurrentProb = 0;
            }
            multi_paramsCtrl.OverallMutationRate = 0.15;
            multi_paramsCtrl.MutateAddLinkProb = 0.08;
            multi_paramsCtrl.MutateAddNeuronProb = 0.01;
            multi_paramsCtrl.MutateWeightsProb = 0.90;
            multi_paramsCtrl.MaxWeight = 8.0;
            multi_paramsCtrl.WeightMutationMaxPower = 0.2;
            multi_paramsCtrl.WeightReplacementMaxPower = 1.0;

            multi_paramsCtrl.MutateActivationAProb = 0.0;
            multi_paramsCtrl.ActivationAMutationMaxPower = 0.5;
            multi_paramsCtrl.MinActivationA = 0.05;
            multi_paramsCtrl.MaxActivationA = 6.0;

            multi_paramsCtrl.MutateNeuronActivationTypeProb = 0.03;

            // Crossover
            multi_paramsCtrl.SurvivalRate = 0.01;
            multi_paramsCtrl.CrossoverRate = 0.01;
            multi_paramsCtrl.InterspeciesCrossoverRate = 0.01;

            multi_paramsCtrl.ActivationFunction_SignedSigmoid_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_Tanh_Prob = 1.0;
            multi_paramsCtrl.ActivationFunction_TanhCubic_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_SignedStep_Prob = 1.0;
            multi_paramsCtrl.ActivationFunction_UnsignedStep_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_SignedGauss_Prob = 1.0;
            multi_paramsCtrl.ActivationFunction_UnsignedGauss_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_Abs_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_SignedSine_Prob = 1.0;
            multi_paramsCtrl.ActivationFunction_UnsignedSine_Prob = 0.0;
            multi_paramsCtrl.ActivationFunction_Linear_Prob = 1.0;

            //// initiate mutli-eval population, named multi_morph/contr_population
            NEAT::Genome multi_morph_genome(0, 5, 10, 6, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0,
                                            multi_params, 10);
            NEAT::Genome multi_contr_genome(0, 7, 10, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0,
                                            multi_paramsCtrl, 2);
            int multi_population_rand_int = randomNum->randInt(0, 10000);
            multi_morph_population = std::make_unique<NEAT::Population>(multi_morph_genome, multi_params, true, 1.0,
                                                                        multi_population_rand_int);
            multi_contr_population = std::make_unique<NEAT::Population>(multi_contr_genome, multi_paramsCtrl, true,
                                                                        1.0, multi_population_rand_int);


            CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
            CPPNGenome::Ptr contrgenome(new CPPNGenome(contr_population->AccessGenomeByIndex(i)));
            CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome, contrgenome));
            ind->set_parameters(parameters);
            population.push_back(ind);
            multi_morph_population->AccessGenomeByIndex(i * eval_num) = morph_population->AccessGenomeByIndex(i);
            multi_contr_population->AccessGenomeByIndex(i * eval_num) = contr_population->AccessGenomeByIndex(i);

            //Generate random controller
            rng.Seed(randomNum->getSeed());
            int population_rand_int = randomNum->randInt(0, 10000);
            NEAT::Genome contr_genome_rand(0, 7, 10, 1, false, NEAT::SIGNED_SIGMOID, NEAT::SIGNED_SIGMOID, 0, paramsCtrl, 2);
            contr_population_rand = std::make_unique<NEAT::Population>(contr_genome_rand, paramsCtrl, true, 1.0, population_rand_int);



            ////Pair random controller with morphology, can be mutated or randomly generated
            for (size_t j = 0; j < eval_num - 1; j++) {
                if(ismultieval_mutate){
                    CPPNGenome(contr_population->AccessGenomeByIndex(i)).mutate();// MAY NOT BE CORRECT
                    CPPNGenome::Ptr new_contrgenome(new CPPNGenome(contr_population->AccessGenomeByIndex(i)));
                    CPPNIndividual::Ptr inner_ind(new CPPNIndividual(morphgenome, new_contrgenome));
                    inner_ind->set_parameters(parameters);
                    population.push_back(inner_ind);
                }
                if(ismultieval_random){
                    CPPNGenome::Ptr new_contrgenome(new CPPNGenome(contr_population_rand->AccessGenomeByIndex(randomNum->randInt(0, params.PopulationSize - 1)))); //Notice that although this is called mutated_contr but it is randomed for testing purpose
                    CPPNIndividual::Ptr inner_ind(new CPPNIndividual(morphgenome, new_contrgenome));
                    inner_ind->set_parameters(parameters);
                    population.push_back(inner_ind);
                }

                multi_morph_population->AccessGenomeByIndex(
                        i * eval_num + j + 1) = morph_population->AccessGenomeByIndex(i);
                //testing
                multi_contr_population->AccessGenomeByIndex(
                        i * eval_num + j + 1) = contr_population->AccessGenomeByIndex(i);
            }
        }
        else if(ismultieval && pop_size_current != pop_size) {
            //// Things are a bit easier for further generations of innner learning loop
            //// update inner population
            population.clear();
            for (int l = 0; l < eval_num * pop_size; l++) {
                CPPNGenome::Ptr multi_morph_genome(new CPPNGenome(multi_morph_population->AccessGenomeByIndex(l)));
                CPPNGenome::Ptr multi_contr_genome(new CPPNGenome(multi_contr_population->AccessGenomeByIndex(l)));
                CPPNIndividual::Ptr ind(new CPPNIndividual(multi_morph_genome, multi_contr_genome));
                ind->set_parameters(parameters);
                population.push_back(ind);
            }
        }
        else{
            //// For outer evolution loop or single loop
            CPPNGenome::Ptr morphgenome(new CPPNGenome(morph_population->AccessGenomeByIndex(i)));
            CPPNGenome::Ptr contrgenome(new CPPNGenome(contr_population->AccessGenomeByIndex(i)));
            CPPNIndividual::Ptr ind(new CPPNIndividual(morphgenome,contrgenome));
            ind->set_parameters(parameters);
            population.push_back(ind);
        }
    }
}

bool MorphNeuro::is_finish()
{   //// Terminate experiment if reach generation limit
    unsigned int maxGenerations = settings::getParameter<settings::Integer>(parameters,"#numberOfGeneration").value;
    return get_generation() > maxGenerations;
}

NEAT::Genome MorphNeuro::loadInd(short genomeID)
{   //// Load specific morphology
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/morphGenome" << genomeID;
    NEAT::Genome indGenome(filepath.str().c_str());
    return indGenome;
}

NEAT::Genome MorphNeuro::loadContrInd(short genomeID)
{   //// load specific controller
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::cout << "Loading genome: " << genomeID << "!" << std::endl;
    std::stringstream filepath;
    filepath << loadExperiment << "/ctrlGenome" << genomeID;
    NEAT::Genome indGenome(filepath.str().c_str());
    return indGenome;
}

std::vector<int> MorphNeuro::listInds()
{   //// Load a list of individuals, can be used in bootstrap method
    std::vector<int> robotList;
    // This code snippet was taken from: https://www.gormanalysis.com/blog/reading-and-writing-csv-files-with-cpp/
    std::string loadExperiment = settings::getParameter<settings::String>(parameters,"#loadExperiment").value;
    std::string bootstrapFile = settings::getParameter<settings::String>(parameters,"#bootstrapFile").value;
    // Create an input filestream
    std::ifstream myFile(loadExperiment+bootstrapFile);
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");
    std::string line;
    int val;

    while(std::getline(myFile, line)){
        // Create a stringstream of the current line
        std::stringstream ss(line);
        // Keep track of the current column index
        int colIdx = 0;
        // Extract each integer
        while(ss >> val) {
            // Add the current integer to the 'colIdx' column's values vector
            robotList.push_back(val);
            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            // Increment the column index
            colIdx++;
        }
    }
    // Close file
    myFile.close();
    return robotList;
}


bool MorphNeuro::update(const Environment::Ptr & env){
    //// update location information in the maze
    endEvalTime = hr_clock::now();
    numberEvaluation++;
    reevaluated++;
    if(simulator_side){
        Individual::Ptr ind = population[currentIndIndex];
        std::dynamic_pointer_cast<CPPNIndividual>(ind)->set_final_position(std::dynamic_pointer_cast<sim::multiTarget>(env)->get_final_position());
        std::vector<double> test_pos = env->get_final_position();
    }
    return true;
}