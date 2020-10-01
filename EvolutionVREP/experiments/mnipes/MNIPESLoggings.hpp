#ifndef MNIPES_LOGGINGS_H
#define MNIPES_LOGGINGS_H

#include "ARE/Logging.h"

#include "M_NIPES.hpp"


namespace are {

class StopCritLog : public Logging
{
public:
    StopCritLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class MorphGenomeLog : public Logging
{
public:
    MorphGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};



class morphDescCartWHDLog : public Logging
{
public:
    morphDescCartWHDLog() : Logging(true){} //Logging at the end of the generation
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


}//are

#endif //CMAES_LOGGINGS_H

