//
// Created by ebb505 on 24/02/2020.
//

#include "ManLog.h"

using namespace are;

void FitnessLog::saveLog(are::EA::Ptr &ea)
{/*
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    savePopFile << generation << ",";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << ea->get_population()[i]->getObjectives()[0] << ",";
    }

    savePopFile << std::endl;
    savePopFile.close();*/
    int populationSize = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;
    std::string repository = settings::getParameter<settings::String>(ea->get_parameters(),"#repository").value;
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    //savePopFile.open(repository + std::string("/") + logFile, std::ios::out | std::ios::ate | std::ios::app);
    savePopFile.open(are::Logging::log_folder + '/' + logFile, std::ios::out | std::ios::ate | std::ios::app);

    if(!savePopFile)
    {
        std::cerr << "unable to open : " << logFile << std::endl;
        return;
    }
    savePopFile << "generation " << generation << ": ,";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        //savePopFile << " ind " << i << ": " << ea->get_population()[i]->getObjectives().back() << ",";
        savePopFile << " ind " << i << ": " << ea->get_population()[i]->getObjectives()[0] << ",";
    }
    float avgFitness = 0;
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        //avgFitness += ea->get_population()[i]->getObjectives().back();
        avgFitness += ea->get_population()[i]->getObjectives()[0];
    }
    avgFitness = avgFitness / ea->get_population().size();
    savePopFile << "avg: ," << avgFitness << ",";
    int bestInd = 0;
    float bestFitness = 0;
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        //if (bestFitness < ea->get_population()[i]->getObjectives().back()) {
        //    bestFitness = ea->get_population()[i]->getObjectives().back();
        //    bestInd = i;
        //}
        if (bestFitness < ea->get_population()[i]->getObjectives()[0]) {
            bestFitness = ea->get_population()[i]->getObjectives()[0];
            bestInd = i;
        }

    }
    //savePopFile << "ind: ," << ea->get_population()[bestInd]->get_individual_id() << ",";
    savePopFile << "ind: ," << generation * populationSize + bestInd << ",";
    savePopFile << "fitness: ," << bestFitness << ",";
    savePopFile << "individuals: ,";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << generation * populationSize + i << ",";
    }
    savePopFile << std::endl;
    savePopFile.close();
}

void MorphDesLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    int pop_size_current = ea->get_population().size();
    int pop_size = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;
    if (pop_size_current == pop_size) {
        for (size_t i = 0; i < ea->get_population().size(); i++) {
            logFileStream << generation * ea->get_population().size() + i << ",";
            Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getMorphDesc();
            for (int j = 0; j < morphDesc.size(); j++) {
                logFileStream << morphDesc(j) << ",";
            }
            logFileStream << std::endl;
        }
    }
    logFileStream.close();
}

void GenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    int pop_size_current = ea->get_population().size();
    int pop_size = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;
    if (pop_size_current == pop_size){
        for(size_t i = 0; i < ea->get_population().size(); i++){
            //NEAT::NeuralNetwork nn;
            //nn = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getGenomeMorphology();
            std::stringstream filepath;
            filepath << Logging::log_folder << "/genome_morph" << generation * ea->get_population().size() + i;
            std::cout << generation * ea->get_population().size() + i << std::endl;
            FILE *file = fopen(filepath.str().c_str(), "w");
            std::dynamic_pointer_cast<CPPNGenome>(std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))
                                                          ->get_morph_genome())->saveGenome(file); // save CPPN
            //nn.Save(file);
            fclose(file);
            //nn.Clear();
        }
    }

}

void ControlLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    int pop_size_current = ea->get_population().size();
    int pop_size = settings::getParameter<settings::Integer>(ea->get_parameters(),"#populationSize").value;
    if (pop_size_current == pop_size) {
        for (size_t i = 0; i < ea->get_population().size(); i++) {
            NEAT::NeuralNetwork nn;
            std::stringstream filepath;
            filepath << Logging::log_folder << "/genome_control" << generation * ea->get_population().size() + i;
            FILE *file = fopen(filepath.str().c_str(), "w");
            std::dynamic_pointer_cast<CPPNGenome>(std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))
                                                          ->get_ctrl_genome())->saveGenome(file); // save CPPN
            //        std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))
            //                                                      ->getGenomeController().Save(file); // save CPPN
            //std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_control()->saveControl(file); // save generated ANN; Note: this line has issue in cluster
            fclose(file);
            //        nn.Clear();
        }
    }
}

void TestsLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << generation * ea->get_population().size() + i << ",";
        std::vector<bool> manRes = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getManRes();
        for(int j = 0; j < manRes.size(); j++){
            logFileStream << manRes[j] << ",";
        }
        //logFileStream << ea->get_population()[i]->getObjectives()[0] << ",";
        logFileStream << std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getManScore() << ",";
        logFileStream << std::endl;
    }
    logFileStream.close();
}

void RawMatrixLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::ofstream rawMatrixFile;
        std::string filepath;
        filepath = Logging::log_folder + "/rawMatrix" + std::to_string(generation * ea->get_population().size() + i) + ".csv";
        rawMatrixFile.open(filepath);
        std::vector<std::vector<float>> rawMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getRawMat();
        for(int j = 0; j < rawMatrix.size(); j++){
            for(int k = 0; k < rawMatrix[j].size(); k++){
                rawMatrixFile << rawMatrix[j][k] << ",";
            }
            rawMatrixFile << std::endl;
        }
        rawMatrixFile.close();
    }
}

void ProtoPhenotypeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::ofstream protoPhenotypeFile;
        std::string filepath;
        filepath = Logging::log_folder + "/protophenotype" + std::to_string(generation * ea->get_population().size() + i) + ".csv";
        protoPhenotypeFile.open(filepath);
        std::vector<std::vector<float>> protoPhenotype = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getProtoPhenotype();
        for(int j = 0; j < protoPhenotype.size(); j++){
            for(int k = 0; k < protoPhenotype[j].size(); k++){
                protoPhenotypeFile << protoPhenotype[j][k] << ",";
            }
            protoPhenotypeFile << std::endl;
        }
        protoPhenotypeFile.close();
    }
}

void morphDescCartWHDLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation * ea->get_population().size() + ind << ",";
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getMorphDesc();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}

void morphDescSymLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation * ea->get_population().size() + ind << ",";
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getSymDesc();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}



