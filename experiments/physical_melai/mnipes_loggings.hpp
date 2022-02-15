#ifndef MNIPES_LOGGINGS_H
#define MNIPES_LOGGINGS_H

#include "ARE/Logging.h"

#include "mnipes.hpp"


namespace are {

class RobotInfoLog : public Logging
{
public:
    RobotInfoLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};


}//are

#endif //CMAES_LOGGINGS_H

