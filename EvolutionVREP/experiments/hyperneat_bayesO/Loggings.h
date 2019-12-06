#ifndef LOGGINGS_H
#define LOGGINGS_H

#include "ARE/Logging.h"
#include "ARE/Individual.h"

namespace are{

class FitnessLog : public Logging
{
public:
    void saveLog(const EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};
}//are
#endif //LOGGINGS_H
