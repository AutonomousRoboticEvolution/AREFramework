#include "ARE/quadrics.hpp"
#include <fstream>
#include <iostream>
#include "ARE/misc/utilities.h"

using namespace are;

quadric_param_t::quadric_param_t(double _a, double _b, double _c,
                                 double _u, double _v,
                                 double _r, double _s, double _t):
        a(_a), b(_b), c(_c), u(_u), v(_v), r(_r), s(_s), t(_t){
}
void quadric_param_t::random(const misc::RandNum::Ptr& rn){
    a += rn->randDouble(-0.5,0.5);
    if(a == 0)
        a = 0.0001;
    b += rn->randDouble(-0.5,0.5);
    if(b == 0)
        b = 0.0001;
    c += rn->randDouble(-0.5,0.5);
    if(c == 0)
        c = 0.0001;
    u += rn->randDouble(-0.5,0.5);
    if(u == 0)
        u = 0.0001;
    v += rn->randDouble(-0.5,0.5);
    if(v == 0)
        v = 0.0001;
    r += rn->randDouble(-0.5,0.5);
    if(r == 0)
        r = 0.0001;
    s += rn->randDouble(-0.5,0.5);
    if(s == 0)
        s = 0.0001;
    t += rn->randDouble(-0.5,0.5);
    if(t == 0)
        t = 0.0001;
}

std::string quadric_param_t::to_string() const{
    std::stringstream sstr;
    sstr << a << ","
         << b << ","
         << c << ","
         << u << ","
         << v << ","
         << r << ","
         << s << ","
         << t;
    return sstr.str();
}

void quadric_param_t::from_string(const std::string& str){
    std::vector<std::string> splitted_str;
    misc::split_line(str,",",splitted_str);
    a = std::stod(splitted_str[0]);
    b = std::stod(splitted_str[1]);
    c = std::stod(splitted_str[2]);
    u = std::stod(splitted_str[3]);
    v = std::stod(splitted_str[4]);
    r = std::stod(splitted_str[5]);
    s = std::stod(splitted_str[6]);
    t = std::stod(splitted_str[7]);
}

std::string sq::quadrics_from_file(std::string &filename, int id){
    std::ifstream ifs(filename);
    std::string line, res;
    std::vector<std::string> split_line;
    while(std::getline(ifs,line)){
        misc::split_line(line,";",split_line); //will have to change for future quadrics file formats
        if(id == std::stoi(split_line[0])){
            res = split_line[1] + std::string(";") +
                split_line[2] + std::string(";") +
                split_line[3] + std::string(";") +
                split_line[4] + std::string(";") +
                split_line[5] + std::string(";") +
                split_line[6] + std::string(";");
            return res;
        }
    }
    std::cerr << "Loading quadrics error --- id " << id << " not found." << std::endl;
    return "";
}

double quadric_mut_params::_mutation_rate = 0.1f;
double quadric_mut_params::_sigma = 0.1f;
double quadric_mut_params::_symmetry_mutation_rate = 0.1f;
