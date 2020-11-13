#ifndef BO_LOGGINGS_H
#define BO_LOGGINGS_H

#include <limbo/serialize/text_archive.hpp>

#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "MFBO.hpp"

namespace are{

class BOBehavDescLog : public Logging
{
public:
    BOBehavDescLog(const std::string &file) : Logging(file,false){}
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
#endif //BO_LOGGINGS_H
