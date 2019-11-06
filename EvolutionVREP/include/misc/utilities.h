#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <list>

namespace misc {

static void split_line(std::string& line, std::string delim, std::list<std::string>& values);

}//misc

#endif //UTILITIES_H
