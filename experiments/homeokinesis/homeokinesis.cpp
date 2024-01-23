#include "homeokinesis.hpp"


using namespace are;
namespace  fs = boost::filesystem;
namespace st = settings;

void HomeoInd::createMorphology(){
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;



    morphology.reset(new sim::FixedMorphology(parameters));
    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    morphology->set_randNum(randNum);


    std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);;



    //    float pos[3];
    //    simGetObjectPosition(std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMainHandle(),-1,pos);
}

void HomeoInd::createController(){
    nb_wheels = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_wheelHandles().size();
    nb_joints = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_jointHandles().size();
    nb_sensors = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_proxHandles().size();

    if(!settings::getParameter<settings::Boolean>(parameters,"#withSensors").value)
        nb_sensors = 0;
    int nb_inputs = nb_sensors + nb_joints*2 + nb_wheels;
    int nb_outputs = nb_joints*2 + nb_joints;
    control =  std::make_shared<hk::Homeokinesis>(nb_inputs,nb_outputs);
}

void HomeoInd::update(double delta_time){
    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    double diff = delta_time/ctrl_freq - std::trunc(delta_time/ctrl_freq);
    if( diff < 0.1){
        std::vector<double> sensors = morphology->update();
        std::vector<double> joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_joints_positions();
        std::vector<double> wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheels_positions();
        std::vector<double> inputs;
        inputs.insert(inputs.begin(),sensors.begin(),sensors.begin() + nb_sensors);
        inputs.insert(inputs.end(),joints.begin(),joints.end());
        inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        std::cout << "inputs: ";
        for(const double& i : inputs)
            std::cout << i << ";";
        std::vector<double> outputs = control->update(inputs);
        std::cout << "outputs: ";
        for(const double& o : outputs)
            std::cout << o << ";";
        std::cout << std::endl;
        morphology->command(outputs);
    }
}

std::string HomeoInd::to_string()
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<HomeoInd>();
    oarch.register_type<NN2CPPNGenome>();
    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void HomeoInd::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<HomeoInd>();
    iarch.register_type<NN2CPPNGenome>();
    iarch.register_type<EmptyGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

void Homeokinesis::init(){
    Genome::Ptr morph_gen = std::make_shared<EmptyGenome>();
    Genome::Ptr ctrl_gen = std::make_shared<EmptyGenome>();

    Individual::Ptr ind = std::make_shared<HomeoInd>(morph_gen,ctrl_gen);
    ind->set_parameters(parameters);
    ind->set_randNum(randomNum);
    population.push_back(ind);

    morph_gen.reset();
    ctrl_gen.reset();
}

bool Homeokinesis::update(const Environment::Ptr &env){
    Individual::Ptr ind = population[currentIndIndex];
    std::dynamic_pointer_cast<HomeoInd>(ind)->set_trajectory(env->get_trajectory());
}

bool Homeokinesis::is_finish(){
    return currentIndIndex >= population.size() - 1;
}
