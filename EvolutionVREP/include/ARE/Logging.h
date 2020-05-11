#ifndef LOGGING_H
#define LOGGING_H

#include <memory>
#include "ARE/EA.h"

namespace are {

class Logging
{
public:

    static void create_log_folder(const std::string &exp_name);
    static std::string log_folder;

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

protected:
    std::string logFile;
    bool endOfGen = true;
};

} //are
#endif //LOGGING_H
