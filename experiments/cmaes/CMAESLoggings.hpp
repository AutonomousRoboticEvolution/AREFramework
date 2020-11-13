#ifndef CMAES_LOGGINGS_H
#define CMAES_LOGGINGS_H

#include "ARE/Logging.h"

#include "CMAES.hpp"


namespace are {

class BehavDescLog : public Logging
{
public:
    BehavDescLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};



}//are

#endif //CMAES_LOGGINGS_H
