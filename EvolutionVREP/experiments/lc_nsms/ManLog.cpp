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
    for (size_t ind = 0; ind < ea->get_population().size(); ind++) {
        savePopFile << ea->get_population()[ind]->getObjectives()[0] << ",";
    }

    savePopFile << std::endl;
    savePopFile.close();
}

void GenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        NEAT::NeuralNetwork nn;
        nn = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getGenome();
        std::stringstream filepath;
        filepath << Logging::log_folder << "/genome" << generation * ea->get_population().size() + ind;
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
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<bool> manRes = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getManRes();
        for(int j = 0; j < manRes.size(); j++){
            logFileStream << manRes[j] << ",";
        }
        //logFileStream << ea->get_population()[ind]->getObjectives()[0] << ",";
        logFileStream << std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getManScore() << ",";
        logFileStream << std::endl;
    }
    logFileStream.close();
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


void ProtoMatrixLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        std::vector<std::vector<std::vector<int>>> graphMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getGraphMatrix();
        for(int x = 0; x < graphMatrix.size(); x++){
            logFileStream << generation * ea->get_population().size() + ind << ",";
            for(int y = 0; y < graphMatrix[x].size(); y++){
                for(int z = 0; z < graphMatrix[x][y].size(); z++){
                    // EB: Ignore voxels in the middle. They are always empty!
                    if(z <= 5 || z >= 7) /// \todo EB: These should be constansts else where!
                        logFileStream << graphMatrix[x][y][z] << ",";
                }
            };
            logFileStream << std::endl;
        }
    }
    logFileStream.close();
}