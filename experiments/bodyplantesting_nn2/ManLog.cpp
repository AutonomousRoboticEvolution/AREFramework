//
// Created by ebb505 on 24/02/2020.
//

#include "ManLog.h"

using namespace are;


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
//        logFileStream << std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getManScore() << ",";
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
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getMorphDesc().getCartDesc();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}

void SkeletonMatrixLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        std::vector<std::vector<std::vector<double>>> skeleton_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_skeleton_matrix();
        filename << "skeleton_matrix_" << std::dynamic_pointer_cast<NN2CPPNGenome>(
                ea->get_population()[i]->get_morph_genome()
        )->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << "Skeleton:" << ",";
        for(int i = 0; i < skeleton_matrix.size(); i++){
            for(int j = 0; j < skeleton_matrix.at(0).size(); j++){
                for(int z = 0; z < skeleton_matrix.at(0).at(0).size(); z++){
                    logFileStream << skeleton_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream.close();
    }
}