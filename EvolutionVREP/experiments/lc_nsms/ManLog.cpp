//
// Created by ebb505 on 24/02/2020.
//

#include "ManLog.h"

using namespace are;

void FitnessLog::saveLog(are::EA::Ptr &ea)
{
    int generation = ea->get_generation();

    std::ofstream savePopFile;
    if(!openOLogFile(savePopFile))
        return;

    savePopFile << generation << ",";
    for (size_t i = 0; i < ea->get_population().size(); i++) {
        savePopFile << ea->get_population()[i]->getObjectives()[0] << ",";
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
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << generation * ea->get_population().size() + i << ",";
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getMorphDesc();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}

void GenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t i = 0; i < ea->get_population().size(); i++){
        NEAT::NeuralNetwork nn;
        nn = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->getGenome();
        std::stringstream filepath;
        filepath << Logging::log_folder << "/genome" << generation * ea->get_population().size() + i;
        FILE *file = fopen(filepath.str().c_str(), "w");
        nn.Save(file);
        fclose(file);
        nn.Clear();
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