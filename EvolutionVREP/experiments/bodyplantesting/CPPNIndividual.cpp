#include "CPPNIndividual.h"

using namespace are;

void CPPNIndividual::update(double delta_time)
{
    /// \todo EB: what is the highest number of outputs?
    std::vector<double> inputs(20, 0.0);
    std::vector<double> tempInputs = morphology->update();

    // Dynamic inputs and outputs handeling
    if(tempInputs.size() > 0){
        for(int i = 0; i < tempInputs.size(); i++) {
            inputs.at(i) = tempInputs[i];
        }
    }

    std::vector<double> outputs = control->update(inputs);
    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->command(outputs);
}

void CPPNIndividual::createMorphology()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
    morphology.reset(new Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
    float init_x = settings::getParameter<settings::Float>(parameters,"#init_x").value;
    float init_y = settings::getParameter<settings::Float>(parameters,"#init_y").value;
    morphology->createAtPosition(init_x,init_y,0.15);
    setGenome();
    setMorphDesc();
    setManRes();
}

void CPPNIndividual::setGenome()
{
    nn = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getGenome();
}

void CPPNIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getMorphDesc();
}

void CPPNIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}

Eigen::VectorXd CPPNIndividual::descriptor(){
    return morphDesc;
}

std::string CPPNIndividual::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<CPPNIndividual>();
    oarch.register_type<CPPNGenome>();
    oarch.register_type<NNParamGenome>();
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    iarch.register_type<CPPNGenome>();
    iarch.register_type<NNParamGenome>();
    iarch >> *this;

    //set parameters and randNum to the genome as it is not contained in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}
