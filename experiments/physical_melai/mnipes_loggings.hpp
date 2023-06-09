#ifndef MNIPES_LOGGINGS_H
#define MNIPES_LOGGINGS_H

#include "ARE/Logging.h"

#include "mnipes.hpp"


namespace are {

class RobotInfoLog : public Logging
{
public:
    RobotInfoLog() : Logging(false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};


class LearnerStateLog : public Logging
{
public:
    LearnerStateLog() : Logging(false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class BestControllerLog : public Logging
{
public:
    BestControllerLog() : Logging(false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ControllerArchiveLog : public Logging
{
public:
    ControllerArchiveLog() : Logging(false){}
    void saveLog(EA::Ptr &ea) override;
    void loadLog(const std::string &file = std::string()) override{}
};


}//are

#endif //CMAES_LOGGINGS_H

