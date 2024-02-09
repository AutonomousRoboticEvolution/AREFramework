#ifndef MEIM_LOGGING_HPP
#define MEIM_LOGGING_HPP

#include "ARE/Logging.h"
#include "meim.hpp"
#include <boost/filesystem.hpp>

namespace are {

class GenomeInfoLog : public Logging
{
public:
	GenomeInfoLog(const std::string &file) : Logging(file,false){}
	void saveLog(EA::Ptr & ea);
	void loadLog(const std::string& logFile){}
};
}

#endif //MEIM_LOGGING_HPP
