#ifndef GEN_LOGGINGS_HPP
#define GEN_LOGGINGS_HPP

#include <ARE/Logging.h>
#include "NNParamGenome.hpp"
#include "NNGenome.hpp"


namespace are {

class NNParamGenomeLog : public Logging
{
public:
    NNParamGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file, EA::Ptr &ea);
    void loadLog(const std::string& log_file){}
};

class NNGenomeLog : public Logging
{
public:
    NNGenomeLog() : Logging(true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea);
    void loadLog(const std::string& log_file, EA::Ptr &ea);
    void loadLog(const std::string& log_file){}
};

}

#endif //GEN_LOGGINGS_HPP
