#ifndef BO_LOGGINGS_HPP
#define BO_LOGGINGS_HPP

#include <limbo/serialize/text_archive.hpp>

#include "ARE/Logging.h"
#include "BO.hpp"

namespace are{

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
#endif //BO_LOGGINGS_HPP

