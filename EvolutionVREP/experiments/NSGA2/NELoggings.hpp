#ifndef HN_LOGGINGS_H
#define HN_LOGGINGS_H

#include "ARE/Logging.h"

#include "NSGA2.hpp"
#include "NSGAIndividual.h"

namespace are {

class BehavDescLog : public Logging
{
public:
    BehavDescLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

}//are

#endif //HN_LOGGINGS_H
