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
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->get_feature_desc().to_eigen_vector();
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
        filename << "matrices_" <<
                ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        std::vector<std::vector<double>> matrix_4d = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_matrix_4d();
        logFileStream << "angle" << ",";
        for(int i = 0; i < matrix_4d.at(0).size(); i++){
            if(matrix_4d.at(0).at(i) < 0.00001 && matrix_4d.at(0).at(i) > -0.00001)
                logFileStream << 0.0 << ",";
            else
                logFileStream << matrix_4d.at(0).at(i) << ",";
        }
        logFileStream << std::endl;
        logFileStream << "skeleton" << ",";
        for(int i = 0; i < matrix_4d.at(1).size(); i++){
            if(matrix_4d.at(1).at(i) < 0.00001 && matrix_4d.at(1).at(i) > -0.00001)
                logFileStream << 0.0 << ",";
            else
                logFileStream << matrix_4d.at(1).at(i) << ",";
        }
        logFileStream << std::endl;
        logFileStream << "wheel" << ",";
        for(int i = 0; i < matrix_4d.at(2).size(); i++){
            if(matrix_4d.at(2).at(i) < 0.00001 && matrix_4d.at(2).at(i) > -0.00001)
                logFileStream << 0.0 << ",";
            else
                logFileStream << matrix_4d.at(2).at(i) << ",";
        }
        logFileStream << std::endl;
        logFileStream << "sensor" << ",";
        for(int i = 0; i < matrix_4d.at(3).size(); i++){
            if(matrix_4d.at(3).at(i) < 0.00001 && matrix_4d.at(3).at(i) > -0.00001)
                logFileStream << 0.0 << ",";
            else
                logFileStream << matrix_4d.at(3).at(i) << ",";
        }
        logFileStream << std::endl;
        logFileStream << "joint" << ",";
        for(int i = 0; i < matrix_4d.at(4).size(); i++){
            if(matrix_4d.at(4).at(i) < 0.00001 && matrix_4d.at(4).at(i) > -0.00001)
                logFileStream << 0.0 << ",";
            else
                logFileStream << matrix_4d.at(4).at(i) << ",";
        }
        logFileStream << std::endl;
        logFileStream << "caster" << ",";
        for(int i = 0; i < matrix_4d.at(5).size(); i++){
            if(matrix_4d.at(5).at(i) < 0.00001 && matrix_4d.at(5).at(i) > -0.00001)
                logFileStream << 0.0 << ",";
            else
                logFileStream << matrix_4d.at(5).at(i) << ",";
        }
        logFileStream << std::endl;
        logFileStream.close();
    }
}

void OrganPositionDescLog::saveLog(EA::Ptr &ea)
{
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        logFileStream << generation * ea->get_population().size() + ind << ",";
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->get_matrix_descriptor().to_eigen_vector();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}
