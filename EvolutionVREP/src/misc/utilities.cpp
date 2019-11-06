#include "misc/utilities.h"

using namespace misc;

void split_line(const std::string& line, const std::string& delim, std::list<std::string>& values){
    size_t pos = 0;
    std::string l = line;
    while ((pos = l.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = l.substr(0, pos);
        values.push_back(p);
        l = l.substr(pos + 1);
    }

    if (!l.empty()) {
        values.push_back(l);
    }
}
