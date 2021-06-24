#ifndef MNIPES_LOGGINGS_H
#define MNIPES_LOGGINGS_H

#include "ARE/Logging.h"

#include "M_NIPES.hpp"


namespace are {

class MorphGenomeLog : public Logging
{
public:
    MorphGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class MorphDescCartWHDLog : public Logging
{
public:
    MorphDescCartWHDLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ControllersLog : public Logging
{
public:
    ControllersLog() : Logging(true){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ControllerArchiveLog : public Logging
{
public:
    ControllerArchiveLog() : Logging(true){}
    void saveLog(EA::Ptr &ea) override;
    void loadLog(const std::string &file = std::string()) override{}
};

}//are

#endif //CMAES_LOGGINGS_H

