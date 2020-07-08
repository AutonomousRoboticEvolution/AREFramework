#ifndef NSGC_LOGGINGS_H
#define NSGC_LOGGINGS_H

#include "ARE/Logging.h"

#include "NSGC.hpp"

namespace are {

class NSGCBehavDescLog : public Logging
{
public:
    NSGCBehavDescLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class NoveltyLog : public Logging
{
public:
    NoveltyLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ArchiveLog : public Logging
{
public:
    ArchiveLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

}//are

#endif //NSGC_LOGGINGS_H

