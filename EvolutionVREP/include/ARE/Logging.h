#ifndef LOGGING_H
#define LOGGING_H

#include "ARE/EA.h"

namespace are {

class Logging
{
public:
    Logging(){}
    Logging(const std::string &file){logFile = file;}
    Logging(const Logging& l) : logFile(l.logFile){}
    virtual ~Logging(){}

    virtual void saveLog(const EA::Ptr& ea) = 0;
    virtual void loadLog(EA::Ptr &ea, const std::string &file = std::string()) = 0;

    //SETTERS && GETTERS
    const std::string &get_logFile(){return logFile;}
    void set_logFile(const std::string& file){logFile = file;}

protected:
    std::string logFile;
};

} //are
#endif //LOGGING_H
