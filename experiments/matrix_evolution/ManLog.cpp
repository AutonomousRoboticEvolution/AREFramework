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
        filename << "matrices_" << std::dynamic_pointer_cast<NN2CPPNGenome>(
                ea->get_population()[i]->get_morph_genome()
        )->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        std::vector<std::vector<std::vector<double>>> temp_matrix;
        temp_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_angle_matrix();
        logFileStream << "Angle:" << ",";
        for(int i = 0; i < temp_matrix.size(); i++){
            for(int j = 0; j < temp_matrix.at(0).size(); j++){
                for(int z = 0; z < temp_matrix.at(0).at(0).size(); z++){
                    logFileStream << temp_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        temp_matrix.clear();
        temp_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_skeleton_matrix();
        logFileStream << "Skeleton" << ",";
        for(int i = 0; i < temp_matrix.size(); i++){
            for(int j = 0; j < temp_matrix.at(0).size(); j++){
                for(int z = 0; z < temp_matrix.at(0).at(0).size(); z++){
                    logFileStream << temp_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        temp_matrix.clear();
        temp_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_wheel_matrix();
        logFileStream << "Wheel" << ",";
        for(int i = 0; i < temp_matrix.size(); i++){
            for(int j = 0; j < temp_matrix.at(0).size(); j++){
                for(int z = 0; z < temp_matrix.at(0).at(0).size(); z++){
                    logFileStream << temp_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        temp_matrix.clear();
        temp_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_sensor_matrix();
        logFileStream << "Sensor" << ",";
        for(int i = 0; i < temp_matrix.size(); i++){
            for(int j = 0; j < temp_matrix.at(0).size(); j++){
                for(int z = 0; z < temp_matrix.at(0).at(0).size(); z++){
                    logFileStream << temp_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        temp_matrix.clear();
        temp_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_joint_matrix();
        logFileStream << "Joint" << ",";
        for(int i = 0; i < temp_matrix.size(); i++){
            for(int j = 0; j < temp_matrix.at(0).size(); j++){
                for(int z = 0; z < temp_matrix.at(0).at(0).size(); z++){
                    logFileStream << temp_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        temp_matrix.clear();
        temp_matrix = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(i))->get_caster_matrix();
        logFileStream << "Caster:" << ",";
        for(int i = 0; i < temp_matrix.size(); i++){
            for(int j = 0; j < temp_matrix.at(0).size(); j++){
                for(int z = 0; z < temp_matrix.at(0).at(0).size(); z++){
                    logFileStream << temp_matrix.at(i).at(j).at(z) << ",";
                }
            }
        }
        logFileStream << std::endl;
        logFileStream.close();
    }
}