#ifndef LOGGINGS_H
#define LOGGINGS_H

#include <limbo/serialize/text_archive.hpp>

#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "EA_HyperNEAT.h"

namespace are{

class FitnessLog : public Logging
{
public:
    FitnessLog(const std::string &file) : Logging(file,true){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class BOLog : public Logging
{
public:
    BOLog(const std::string &file) : Logging(file,false){}
    void saveLog(EA::Ptr &ea);
    void loadLog(const std::string& logFile){}
};


class LearnerSerialLog : public Logging
{
public:
    LearnerSerialLog(const std::string &file) : Logging(file,false){}
    void saveLog(EA::Ptr &ea);
    void loadLog(const std::string& logFile){}
};


}//are
#endif //LOGGINGS_H
