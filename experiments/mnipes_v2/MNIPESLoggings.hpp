#ifndef MNIPES_LOGGINGS_H
#define MNIPES_LOGGINGS_H

#include "ARE/Logging.h"
#include <boost/filesystem.hpp>
#include "M_NIPES.hpp"


namespace are {

class GenomeInfoLog : public Logging
{
public:
    GenomeInfoLog(const std::string &file) : Logging(file,false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class MorphDescCartWHDLog : public Logging
{
public:
    MorphDescCartWHDLog(const std::string &file) : Logging(file,false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ControllersLog : public Logging
{
public:
    ControllersLog() : Logging(false){}
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& logFile){}
};

class ControllerArchiveLog : public Logging
{
public:
    ControllerArchiveLog() : Logging(false){}
    void saveLog(EA::Ptr &ea) override;
    void loadLog(const std::string &file = std::string()) override{}
};

class GenomesPoolLog : public Logging
{
public:
    GenomesPoolLog(const std::string &file) : Logging(file, false){}
    void saveLog(EA::Ptr &ea) override;
    void loadLog(const std::string &file = std::string()) override{}
};


class BestGenomesArchiveLog : public Logging
{
public:
    BestGenomesArchiveLog(const std::string &file) : Logging(file, false){}
    void saveLog(EA::Ptr &ea) override;
    void loadLog(const std::string &file = std::string()) override{}
};


}//are

#endif //CMAES_LOGGINGS_H

