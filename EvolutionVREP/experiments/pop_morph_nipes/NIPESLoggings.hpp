#ifndef NIPES_LOGGINGS_H
#define NIPES_LOGGINGS_H

#include "ARE/Logging.h"

#include "PMNIPES.hpp"


namespace are {

namespace morph_log {

class StopCritLog : public Logging
{
public:
    StopCritLog(const std::string &file) : Logging(file,true)
      , log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};

class FitnessLog : public Logging
{
public:
    FitnessLog(const std::string &file) : Logging(file,true), log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};

class EvalTimeLog : public Logging
{
public:
    EvalTimeLog(const std::string &file) : Logging(file,false), log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};

template <class ind_t>
class TrajectoryLog : public Logging
{
public:
    TrajectoryLog() : Logging(true){}
    void saveLog(EA::Ptr& ea){
        std::ofstream ofs;
        int generation = ea->get_generation();
        for(size_t i = 0; i < ea->get_population().size(); i++){
            std::vector<are::waypoint> traj = std::dynamic_pointer_cast<ind_t>(ea->getIndividual(i))->get_trajectory();
            std::stringstream filepath;
            filepath << "/traj_" << generation << "_" << i;
            logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + filepath.str();

            if(!openOLogFile(ofs))
                return;
            for(const are::waypoint& wp: traj)
                ofs << wp.to_string() << std::endl;
            ofs.close();
        }
    }
    void loadLog(const std::string &file){}
};

template <class ind_t>
class BehavDescLog : public Logging
{
public:
    BehavDescLog(const std::string &file) : Logging(file,true), log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){

        logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + log_file;

        std::ofstream logFileStream;
        if(!openOLogFile(logFileStream))
            return;

        int generation = ea->get_generation();

        logFileStream << "generation," << generation << ",";
        for(size_t i = 0; i < ea->get_population().size(); i++){
            logFileStream << i << ",";
            for(const double &pos :
                std::dynamic_pointer_cast<ind_t>(ea->getIndividual(i))->get_final_position())
                logFileStream << pos << ",";
        }
        logFileStream << std::endl;

        logFileStream.close();
    }
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};

template <class ind_t>
class EnergyCostLog : public Logging
{
public:
    EnergyCostLog(const std::string &file) : Logging(file,true), log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int generation = ea->get_generation();

        logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + log_file;

        std::ofstream savePopFile;
        if(!openOLogFile(savePopFile))
            return;

        savePopFile << generation << "," << ea->get_population().size() << "," ;
        for (size_t i = 0; i < ea->get_population().size(); i++) {
            savePopFile << std::dynamic_pointer_cast<ind_t>(ea->getIndividual(i))->get_energy_cost() << ",";
        }

        savePopFile << std::endl;
        savePopFile.close();
    }
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};

template <class ind_t>
class SimTimeLog : public Logging
{
public:
    SimTimeLog(const std::string &file) : Logging(file,true), log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int generation = ea->get_generation();
        logFile = static_cast<PMNIPES*>(ea.get())->subFolder() + std::string("/") + log_file;

        std::ofstream savePopFile;
        if(!openOLogFile(savePopFile))
            return;

        savePopFile << generation << "," << ea->get_population().size() << "," ;
        for (size_t i = 0; i < ea->get_population().size(); i++) {
            savePopFile << std::dynamic_pointer_cast<ind_t>(ea->getIndividual(i))->get_sim_time() << ",";
        }

        savePopFile << std::endl;
        savePopFile.close();
    }
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};


class NNParamGenomeLog : public Logging
{
public:
    NNParamGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file){}
};



template <class ea_t>
class ArchiveLog : public Logging
{
public:
    ArchiveLog(const std::string &file) : Logging(file,true), log_file(file){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int generation = ea->get_generation();
        logFile = static_cast<ea_t*>(ea.get())->subFolder() + std::string("/") + log_file;

        std::ofstream savePopFile;
        if(!openOLogFile(savePopFile))
            return;

        Eigen::VectorXd desc;

        savePopFile << generation << ";" << static_cast<ea_t*>(ea.get())->get_archive().size() << ";";
        for (size_t i = 0; i < static_cast<ea_t*>(ea.get())->get_archive().size(); i++) {
            desc = static_cast<ea_t*>(ea.get())->get_archive()[i];
            for(size_t j = 0; j < desc.rows() - 1 ; j++)
                savePopFile << desc(j) << ",";
            savePopFile << desc(desc.rows()-1) << ";";
        }
        savePopFile << std::endl;
        savePopFile.close();
    }
    void loadLog(const std::string& logFile){}
private:
    std::string log_file;
};
<<<<<<< HEAD

class ControllerArchiveLog : public Logging
{
public:
    ControllerArchiveLog() : Logging(true){}
    void saveLog(EA::Ptr &ea) override;
    void loadLog(const std::string &file = std::string()) override{}
};

=======
>>>>>>> add new experiment : pop_morph_nipes
}//morph_log
}//are

#endif //CMAES_LOGGINGS_H

