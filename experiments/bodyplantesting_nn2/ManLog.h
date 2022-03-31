//
// Created by ebb505 on 24/02/2020.
//

#ifndef ER_MANLOG_H
#define ER_MANLOG_H

#include "ARE/Logging.h"
#include "ARE/Individual.h"
#include "ARE/EA.h"
#include "BodyPlanTesting.h"

namespace are {

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

class morphDescCartWHDLog : public Logging
{
public:
    morphDescCartWHDLog(const std::string &file) : Logging(file, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class SkeletonMatrixLog : public Logging
{
public:
    SkeletonMatrixLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class OrganPositionDescLog: public Logging
{
public:
    OrganPositionDescLog(const std::string &file) : Logging(file, true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};
}//are

#endif //ER_MANLOG_H
