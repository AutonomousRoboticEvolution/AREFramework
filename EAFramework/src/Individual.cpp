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

void Individual::crossover(const Individual::Ptr &partner, Individual *child){
    morphGenome->crossover(partner->get_morph_genome(),child->get_morph_genome());
}

void Individual::symmetrical_crossover(const Individual::Ptr &partner, Individual *child1, Individual *child2){
    morphGenome->symmetrical_crossover(partner->get_morph_genome(),child1->get_morph_genome(),child2->get_morph_genome());
}

std::string Individual::to_string() const
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

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
