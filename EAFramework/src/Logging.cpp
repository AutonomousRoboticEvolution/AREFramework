#include "ARE/Logging.h"

#include <chrono>
#include <boost/filesystem.hpp>
#include <random>

std::string are::Logging::log_folder = "";

void are::Logging::create_log_folder(const std::string &exp_name){
    std::random_device rd;

    typedef std::chrono::duration<double,std::milli> milli_sec;
    std::chrono::time_point<std::chrono::high_resolution_clock,milli_sec> milli =
            std::chrono::time_point_cast<milli_sec>(std::chrono::high_resolution_clock::now());
    double time_milli = milli.time_since_epoch().count();
    time_milli = time_milli/(10000.f);
    time_milli = time_milli - static_cast<int>(time_milli);
    time_milli = std::trunc(time_milli*10000.f);
    std::time_t present_time = std::time(nullptr);
    std::tm* date = std::localtime(&present_time);
    std::stringstream stream;
    stream << date->tm_mday
        << "_" << date->tm_mon
        << "_" << date->tm_hour
        << "-" << date->tm_min
        << "-" << date->tm_sec
        << "-" << time_milli
        << "-" << rd();

    set_log_folder(exp_name + "_" + stream.str());
    create_folder(exp_name + "_" + stream.str());
}
void are::Logging::set_log_folder(const std::string& name){
    log_folder = name;
}
void are::Logging::create_folder(const std::string &name){
    if(!boost::filesystem::exists(name))
        boost::filesystem::create_directory(name);
}


bool are::Logging::openOLogFile(std::ofstream &logFileStream){
    logFileStream.open(Logging::log_folder + std::string("/")  + logFile, std::ios::out | std::ios::ate | std::ios::app);

    if(!logFileStream)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/")  + logFile << std::endl;
        return false;
    }

    return true;
}


bool are::Logging::openILogFile(std::ifstream &logFileStream){
    logFileStream.open(Logging::log_folder + std::string("/")  + logFile);

    if(!logFileStream)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/")  + logFile << std::endl;
        return false;
    }

    return true;
}

bool are::Logging::openOLogFile(std::ofstream &logFileStream, const std::string &log_file){
    logFileStream.open(Logging::log_folder + std::string("/")  + log_file, std::ios::out | std::ios::ate | std::ios::app);

    if(!logFileStream)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/")  + log_file << std::endl;
        return false;
    }

    return true;
}

bool are::Logging::openILogFile(std::ifstream &logFileStream, const std::string &log_file){
    logFileStream.open(Logging::log_folder + std::string("/")  + log_file);

    if(!logFileStream)
    {
        std::cerr << "unable to open : " << Logging::log_folder + std::string("/")  + log_file << std::endl;
        return false;
    }

    return true;
}

void are::FitnessLog::saveLog(are::EA::Ptr &ea)
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

void are::EvalTimeLog::saveLog(EA::Ptr &ea){

    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream))
        return;

    int nbEval = ea->get_numberEvaluation();
    std::chrono::nanoseconds eval_time = ea->getEvalCompTime();

    logFileStream << nbEval <<  ", " << eval_time.count() << std::endl;

    logFileStream.close();
}
