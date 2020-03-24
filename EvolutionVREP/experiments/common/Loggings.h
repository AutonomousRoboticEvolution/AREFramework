#ifndef LOGGINGS_H
#define LOGGINGS_H

#include "ARE/Logging.h"
#include "ARE/Individual.h"

#include "ARE/EA.h"

namespace are{

class FitnessLog : public Logging
{
public:
    FitnessLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class EvalTimeLog : public Logging
{
public:
    EvalTimeLog(const std::string &file) : Logging(file,false){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

}//are
#endif //LOGGINGS_H
