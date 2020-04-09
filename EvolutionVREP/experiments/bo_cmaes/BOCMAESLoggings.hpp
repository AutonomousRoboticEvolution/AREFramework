#ifndef BOCMAES_LOGGINGS_H
#define BOCMAES_LOGGINGS_H

#include "ARE/Logging.h"

#include "BOCMAES.hpp"


namespace are {

class BehavDescLog : public Logging
{
public:
    BehavDescLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class StopCritLog : public Logging
{
public:
    StopCritLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class NoveltyLog : public Logging
{
public:
    NoveltyLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ArchiveLog : public Logging
{
public:
    ArchiveLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

}//are

#endif //OCMAES_LOGGINGS_H
