#include <ARE/Individual.h>

using namespace are;

Individual::Individual(const Genome::Ptr &morph_gen,const Genome::Ptr &ctrl_gen) :
    morphGenome(morph_gen),ctrlGenome(ctrl_gen)
{
//    init();
}

Individual::~Individual(){
    morphGenome.reset();
    ctrlGenome.reset();
    morphology.reset();
    control.reset();
}

std::string Individual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch << *this;
    return sstream.str();
}

void Individual::from_string(const std::string &str){
    std::cout << "Individual" <<std::endl;
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch >> *this;
}
