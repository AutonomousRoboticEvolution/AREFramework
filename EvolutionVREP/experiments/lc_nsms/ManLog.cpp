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

void RawMatrixLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        std::ofstream rawMatrixFile;
        std::string filepath;
        filepath = Logging::log_folder + "/rawMatrix" + std::to_string(generation * ea->get_population().size() + ind) + ".csv";
        rawMatrixFile.open(filepath);
        std::vector<std::vector<float>> rawMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getRawMat();
        for(int j = 0; j < rawMatrix.size(); j++){
            for(int k = 0; k < rawMatrix[j].size(); k++){
                rawMatrixFile << rawMatrix[j][k] << ",";
            }
            rawMatrixFile << std::endl;
        }
        rawMatrixFile.close();
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

void BinCPPNMatrixLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> cppnBoolSkeletonMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getCPPNBoolSkeletonMatrix();
        for(int x = 0; x < cppnBoolSkeletonMatrix.size(); x++){
            for(int y = 0; y < cppnBoolSkeletonMatrix[x].size(); y++){
                for(int z = 0; z < cppnBoolSkeletonMatrix[x][y].size(); z++){
                    logFileStream << cppnBoolSkeletonMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> cppnBoolWheelMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getCPPNBoolWheelMatrix();
        for(int x = 0; x < cppnBoolWheelMatrix.size(); x++){
            for(int y = 0; y < cppnBoolWheelMatrix[x].size(); y++){
                for(int z = 0; z < cppnBoolWheelMatrix[x][y].size(); z++){
                    logFileStream << cppnBoolWheelMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> cppnBoolSensorMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getCPPNBoolSensorMatrix();
        for(int x = 0; x < cppnBoolSensorMatrix.size(); x++){
            for(int y = 0; y < cppnBoolSensorMatrix[x].size(); y++){
                for(int z = 0; z < cppnBoolSensorMatrix[x][y].size(); z++){
                    logFileStream << cppnBoolSensorMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> cppnBoolJointMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getCPPNBoolJointMatrix();
        for(int x = 0; x < cppnBoolJointMatrix.size(); x++){
            for(int y = 0; y < cppnBoolJointMatrix[x].size(); y++){
                for(int z = 0; z < cppnBoolJointMatrix[x][y].size(); z++){
                    logFileStream << cppnBoolJointMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> cppnBoolCasterMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getCPPNBoolCasterMatrix();
        for(int x = 0; x < cppnBoolCasterMatrix.size(); x++){
            for(int y = 0; y < cppnBoolCasterMatrix[x].size(); y++){
                for(int z = 0; z < cppnBoolCasterMatrix[x][y].size(); z++){
                    logFileStream << cppnBoolCasterMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}

void InterMatrixLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> interWheelMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getInterWheelMatrix();
        for(int x = 0; x < interWheelMatrix.size(); x++){
            for(int y = 0; y < interWheelMatrix[x].size(); y++){
                for(int z = 0; z < interWheelMatrix[x][y].size(); z++){
                    logFileStream << interWheelMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> interSensorMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getInterSensorMatrix();
        for(int x = 0; x < interSensorMatrix.size(); x++){
            for(int y = 0; y < interSensorMatrix[x].size(); y++){
                for(int z = 0; z < interSensorMatrix[x][y].size(); z++){
                    logFileStream << interSensorMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> interJointMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getInterJointMatrix();
        for(int x = 0; x < interJointMatrix.size(); x++){
            for(int y = 0; y < interJointMatrix[x].size(); y++){
                for(int z = 0; z < interJointMatrix[x][y].size(); z++){
                    logFileStream << interJointMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream << generation * ea->get_population().size() + ind << ",";
        std::vector<std::vector<std::vector<bool>>> interCasterMatrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getInterCasterMatrix();
        for(int x = 0; x < interCasterMatrix.size(); x++){
            for(int y = 0; y < interCasterMatrix[x].size(); y++){
                for(int z = 0; z < interCasterMatrix[x][y].size(); z++){
                    logFileStream << interCasterMatrix[x][y][z] << ",";
                }
            }
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}