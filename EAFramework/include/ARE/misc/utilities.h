#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <list>

namespace are {

namespace misc {

void split_line(const std::string &line,const std::string &delim,
                std::list<std::string> &values);

}//misc

}//are

#endif //UTILITIES_H
