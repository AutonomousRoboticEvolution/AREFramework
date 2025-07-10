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

void Individual::update(double delta_time){
    bool use_joint_feedback = settings::getParameter<settings::Boolean>(parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(parameters,"#useWheelFeedback").value;
    // double input_noise_lvl = settings::getParameter<settings::Double>(parameters,"#inputNoiseLevel").value;
    // double output_noise_lvl = settings::getParameter<settings::Double>(parameters,"#outputNoiseLevel").value;
    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;
    double max_velo = settings::getParameter<settings::Double>(parameters,"#maxVelocity").value;

    if( fabs(sum_ctrl_freq - ctrl_freq) < 0.0001){
        act_obs_sample aos;
        sum_ctrl_freq = 0;
        // - Retrieve sensors, joints and wheels values
        std::vector<double> inputs = morphology->update();

        if(use_joint_feedback){
            std::vector<double> joints = morphology->get_joints_positions();
            for(double &j: joints)
                j = 2.*j/M_PI;
            inputs.insert(inputs.end(),joints.begin(),joints.end());
        }
        if(use_wheel_feedback){
            std::vector<double> wheels = morphology->get_wheels_velocities();
            for(double &w: wheels)
                w = w/max_velo;
            inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        }
        for(double& i: inputs)
            i = misc::round_at_precision(i,2);

        aos.observation = inputs;


        std::vector<double> outputs = control->update(inputs);
        for(double& o: outputs)
            o = misc::round_at_precision(o,2);
        aos.next_action = outputs;

        morphology->command(outputs);
        rollout.push_back(aos);
    }
    sum_ctrl_freq += settings::getParameter<settings::Float>(parameters,"#timeStep").value;
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

std::string act_obs_sample::to_string() const{
    std::stringstream sstr;
    sstr << observation[0];
    for(size_t i = 1; i < observation.size(); i++)
        sstr << "," << observation[i];
    sstr << ";" << next_action[0];
    for(size_t i = 1; i < next_action.size(); i++)
        sstr << "," << next_action[i];
    return sstr.str();
}
