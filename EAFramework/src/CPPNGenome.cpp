#include "ARE/CPPNGenome.h"

using namespace are;
using namespace are::neat_cppn;

void NbrConnNeurLog::saveLog(EA::Ptr &ea){
    int generation = ea->get_generation();
    std::ofstream logFileStream;
    if(!openOLogFile(logFileStream,logFile))
        return;
    for(size_t i = 0; i < ea->get_population().size(); i++){
        logFileStream << generation*100 + i << ","
                      << std::dynamic_pointer_cast<CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_neurons()
                      << ","
                      << std::dynamic_pointer_cast<CPPNGenome>(ea->get_population()[i]->get_morph_genome())->get_nb_connections()
                      << std::endl;
    }
    logFileStream.close();
}
