#include "CPPNIndividual.h"

using namespace are;

CPPNIndividual::CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
    Individual(morph_gen,ctrl_gen)
{
//    createMorphology();
//    createController();
}

Individual::Ptr CPPNIndividual::clone()
{
    return std::make_shared<CPPNIndividual>(*this);
}

void CPPNIndividual::update(double delta_time)
{

}

void CPPNIndividual::createMorphology()
{
    NEAT::Genome gen =
            std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
    morphology.reset(new Morphology_CPPNMatrix(parameters));
    NEAT::NeuralNetwork nn;
    gen.BuildPhenotype(nn);
    std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
    morphology->createAtPosition(0,0,0.12);
    setGenome();
    setMorphDesc();
    setManRes();
//    setRawMat();
    setManScore();
    setGraphMatrix();
//    setCPPNBoolSkeletonMatrix();
//    setCPPNBoolWheelMatrix();
//    setCPPNBoolSensorMatrix();
//    setCPPNBoolJointMatrix();
//    setCPPNBoolCasterMatrix();
//    setInterWheelMatrix();
//    setInterSensorMatrix();
//    setInterJointMatrix();
//    setInterCasterMatrix();
}

void CPPNIndividual::createController()
{
    //control.reset(new FixedController);
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

void CPPNIndividual::setRawMat()
{
    rawMat = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getRawMatrix();
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
//    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void CPPNIndividual::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<CPPNIndividual>();
    iarch.register_type<CPPNGenome>();
//    iarch.register_type<EmptyGenome>();
    iarch >> *this;
}

void CPPNIndividual::setManScore()
{
    manScore = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getManScore();
}

void CPPNIndividual::setGraphMatrix()
{
    graphMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getGraphMatrix();
}

void CPPNIndividual::setCPPNBoolSkeletonMatrix()
{
    skeletonMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getCPPNBoolSkeletonMatrix();
}
void CPPNIndividual::setCPPNBoolWheelMatrix()
{
    wheelMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getCPPNBoolWheelMatrix();
}
void CPPNIndividual::setCPPNBoolSensorMatrix()
{
    sensorMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getCPPNBoolSensorMatrix();
}
void CPPNIndividual::setCPPNBoolJointMatrix()
{
    jointMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getCPPNBoolJointMatrix();
}
void CPPNIndividual::setCPPNBoolCasterMatrix()
{
    casterMatrix =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getCPPNBoolCasterMatrix();
}
void CPPNIndividual::setInterWheelMatrix()
{
    wheelInter =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getInterWheelMatrix();
}
void CPPNIndividual::setInterSensorMatrix()
{
    sensorInter =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getInterSensorMatrix();
}
void CPPNIndividual::setInterJointMatrix()
{
    jointInter =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getInterJointMatrix();
}
void CPPNIndividual::setInterCasterMatrix()
{
    casterInter =  std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getInterCasterMatrix();
}