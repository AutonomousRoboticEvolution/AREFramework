#ifndef NIPES_LOGGINGS_H
#define NIPES_LOGGINGS_H

#include <memory>
#include "ARE/Logging.h"
#include "env_settings.hpp"

namespace are {

class BestIndividualLog : public Logging
{
public:
    BestIndividualLog() : Logging(){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};



}//are

#endif //CMAES_LOGGINGS_H

