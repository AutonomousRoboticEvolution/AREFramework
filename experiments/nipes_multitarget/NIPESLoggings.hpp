#ifndef NIPES_LOGGINGS_H
#define NIPES_LOGGINGS_H

#include "ARE/Logging.h"

#include "NIPES.hpp"


namespace are {

class StopCritLog : public Logging
{
public:
    StopCritLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};



}//are

#endif //CMAES_LOGGINGS_H

