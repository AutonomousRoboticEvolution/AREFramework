#include "ARE/Logging.h"

#include <chrono>
#include <boost/filesystem.hpp>

std::string are::Logging::log_folder = "";

void are::Logging::create_log_folder(const std::string &exp_name){
    typedef std::chrono::duration<double,std::milli> milli_sec;
    std::chrono::time_point<std::chrono::high_resolution_clock,milli_sec> milli = std::chrono::time_point_cast<milli_sec>(std::chrono::high_resolution_clock::now());
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
        << "-" << time_milli;

    log_folder = exp_name + stream.str();

    if(!boost::filesystem::exists(log_folder))
        boost::filesystem::create_directory(log_folder);

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