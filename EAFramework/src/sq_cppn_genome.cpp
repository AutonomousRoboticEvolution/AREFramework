#include "ARE/nn2/sq_cppn_genome.hpp"

using namespace are;
using namespace are::sq_cppn;

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

void QuadricsLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        if(!openOLogFile(logFileStream, logFile))
            return;
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ";"
                      << std::dynamic_pointer_cast<SQCPPNGenome>(
                            ea->get_population()[i]->get_morph_genome()
                        )->get_quadric().to_string() << std::endl;
        logFileStream.close();
    }
}

void SQCPPNGenomeLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "morph_genome_" << ea->get_population()[i]->get_morph_genome()->id();
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        logFileStream << std::dynamic_pointer_cast<SQCPPNGenome>(
                             ea->get_population()[i]->get_morph_genome()
                             )->to_string();
        logFileStream.close();
    }
}

void NbrConnNeurLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()
                      << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()
                      << std::endl;
    }
    logFileStream.close();
}

void ParentingLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << ea->get_population()[i]->get_morph_genome()->id() << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[0]
                      << ","
                      << std::dynamic_pointer_cast<SQCPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_parents_ids()[1]
                      << std::endl;
    }
    logFileStream.close();
}

void GraphVizLog::saveLog(EA::Ptr &ea){
    std::ofstream logFileStream;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        std::stringstream filename;
        filename << "cppn_" << std::dynamic_pointer_cast<SQCPPNGenome>(
                                   ea->get_population()[i]->get_morph_genome()
                                   )->id() << ".dot";
        if(!openOLogFile(logFileStream, filename.str()))
            return;
        nn2_cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(
                              ea->get_population()[i]->get_morph_genome()
                              )->get_cppn();
        cppn.write_dot(logFileStream);
        logFileStream.close();
    }
}
