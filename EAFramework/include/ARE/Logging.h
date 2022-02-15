#ifndef LOGGING_H
#define LOGGING_H

#include <memory>
#include "ARE/EA.h"

namespace are {

class Logging
{
public:

    static void create_log_folder(const std::string &exp_name);
    static void create_folder(const std::string &name);
    static void set_log_folder(const std::string&);
    static std::string log_folder;

    // Takes an arbitrary string input and saves it as a new file in the log_folder:
    static void saveStringToFile(const std::string &fileName, const std::string &data);

    typedef std::shared_ptr<Logging> Ptr;
    typedef std::shared_ptr<const Logging> ConstPtr;
    typedef void (Factory) (std::vector<Logging::Ptr>&,
                            const settings::ParametersMapPtr &);

    Logging() : endOfGen(true){}
    Logging(bool eog) : endOfGen(eog){}
    Logging(const std::string &file, bool eog) : endOfGen(eog){logFile = file;}
    Logging(const Logging& l) : logFile(l.logFile), endOfGen(l.endOfGen){}
    virtual ~Logging(){}

    virtual void saveLog(EA::Ptr& ea) = 0;
    virtual void loadLog(const std::string &file = std::string()) = 0;
    bool openOLogFile(std::ofstream&);
    bool openILogFile(std::ifstream&);

    bool openOLogFile(std::ofstream&,const std::string &log_file);
    bool openILogFile(std::ifstream&,const std::string &log_file);

    //SETTERS && GETTERS
    const std::string &get_logFile(){return logFile;}
    void set_logFile(const std::string& file){logFile = file;}
    bool isEndOfGen(){return endOfGen;}
    void set_end_of_gen(bool eog){endOfGen = eog;}

protected:
    std::string logFile;
    bool endOfGen = true;
};

class FitnessLog : public Logging
{
public:
    FitnessLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class EvalTimeLog : public Logging
{
public:
    EvalTimeLog(const std::string &file) : Logging(file,false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

template <class ind_t>
class BehavDescLog : public Logging
{
public:
    BehavDescLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
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
};

template <class ind_t>
class EnergyCostLog : public Logging
{
public:
    EnergyCostLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int generation = ea->get_generation();

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
};

template <class ind_t>
class SimTimeLog : public Logging
{
public:
    SimTimeLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int generation = ea->get_generation();

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
            logFile = filepath.str();

            if(!openOLogFile(ofs))
                return;
            for(const are::waypoint& wp: traj)
                ofs << wp.to_string() << std::endl;
            ofs.close();
        }
    }
    void loadLog(const std::string &file){}
};


} //are
#endif //LOGGING_H
