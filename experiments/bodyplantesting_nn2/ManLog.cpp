//
// Created by ebb505 on 24/02/2020.
//

#include "ManLog.h"

using namespace are;


void GenomeLog::saveLog(EA::Ptr &ea)
{
    int generation = ea->get_generation();
    for(size_t ind = 0; ind < ea->get_population().size(); ind++){
        std::stringstream filepath;
        filepath << Logging::log_folder << "/morphGenome" << generation * ea->get_population().size() + ind;

        const Genome::Ptr morphGenome = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->get_morph_genome();
        std::ofstream ofstr(filepath.str());
        boost::archive::text_oarchive oarch(ofstr);
        oarch << std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
        ofstr.close();
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
        Eigen::VectorXd morphDesc = std::dynamic_pointer_cast<CPPNIndividual>(ea->getIndividual(ind))->getMorphDesc();
        for(int j = 0; j < morphDesc.size(); j++){
            logFileStream << morphDesc(j) << ",";
        }
        logFileStream << std::endl;
    }
    logFileStream.close();
}
