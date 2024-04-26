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

    Logging() : end_of_gen(true), end_of_run(false){}
    Logging(bool eog, bool eor = false) : end_of_gen(eog), end_of_run(eor){}
    Logging(const std::string &file, bool eog = true) : end_of_gen(eog){logFile = file;}
    Logging(const Logging& l) :
        logFile(l.logFile),
        end_of_gen(l.end_of_gen),
        end_of_run(l.end_of_run){}
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
    bool isEndOfGen(){return end_of_gen;}
    bool isEndOfRun(){return end_of_run;}
    void set_end_of_gen(bool eog){end_of_gen = eog;}

protected:
    std::string logFile;
    bool end_of_gen = true;
    bool end_of_run = false;
};

class FitnessLog : public Logging
{
public:
    FitnessLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
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
    TrajectoryLog() : Logging(true){} //Logging at the end of the generation
    TrajectoryLog(double ot) :
        objective_threshold(ot),
        Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr& ea){
        std::ofstream ofs;
        int generation = ea->get_generation();
        for(size_t i = 0; i < ea->get_population().size(); i++){
            auto &ind = ea->get_population()[i];
            if(ind->getObjectives()[0] > objective_threshold){
                std::vector<are::waypoint> traj = std::dynamic_pointer_cast<ind_t>(ind)->get_trajectory();
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
    }
    void loadLog(const std::string &file){}
private:
    double objective_threshold = 0;
};


} //are
#endif //LOGGING_H
