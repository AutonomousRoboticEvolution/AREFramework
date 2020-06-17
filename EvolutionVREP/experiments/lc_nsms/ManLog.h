//
// Created by ebb505 on 24/02/2020.
//

#ifndef ER_MANLOG_H
#define ER_MANLOG_H

#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/EA.h"
#include "LC_NSMS.h"

namespace are {

class FitnessLog : public Logging
{
public:
    FitnessLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};


class MorphDesLog : public Logging
{
public:
    MorphDesLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class GenomeLog : public Logging
{
public:
    GenomeLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class TestsLog : public Logging
{
public:
    TestsLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class RawMatrixLog : public Logging
{
public:
    RawMatrixLog(const std::string &file) : Logging(file, true){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

}//are

#endif //ER_MANLOG_H
