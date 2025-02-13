#include "visualization.hpp"
#include "ARE/NNParamGenome.hpp"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/are_morphology.hpp"
#include "ARE/nn2/NN2Settings.hpp"
#include "simulatedER/nn2/sq_cppn_genome.hpp"

using namespace are;
namespace  fs = boost::filesystem;
namespace st = settings;

void VisuInd::createMorphology(){
    std::string fixed_morph_path =  settings::getParameter<settings::String>(parameters,"#robotPath").value;
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    std::string manual_design = settings::getParameter<settings::String>(parameters,"#manualDesignFile").value;
    bool use_quadric = settings::getParameter<settings::Boolean>(parameters,"#useQuadric").value;

    // if(manual_design != "None"){
    //     morphology = std::make_shared<sim::ManuallyDesignedMorphology>(parameters);
    //     morphology->set_randNum(randNum);
    //     std::dynamic_pointer_cast<sim::ManuallyDesignedMorphology>(morphology)->set_list_of_voxels(std::dynamic_pointer_cast<ManualDesign>(morphGenome)->list_of_voxels);
    //     std::dynamic_pointer_cast<sim::ManuallyDesignedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
    //     return;
    // }

    if(fixed_morph_path == "None"){
        if(!use_quadric){
            morphology = std::make_shared<sim::CPPNMorphology>(parameters);
            morphology->set_randNum(randNum);
            nn2_cppn_t gen = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
            std::dynamic_pointer_cast<sim::CPPNMorphology>(morphology)->set_cppn(gen);
        }else{
            morphology = std::make_shared<sim::SQCPPNMorphology>(parameters);
            morphology->set_randNum(randNum);
            sq_cppn::cppn_t cppn = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_cppn();
            quadric_t<quadric_params> quadric = std::dynamic_pointer_cast<SQCPPNGenome>(morphGenome)->get_quadric();
            std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->set_cppn(cppn);
            std::dynamic_pointer_cast<sim::SQCPPNMorphology>(morphology)->set_quadric(quadric);
        }
        int id = settings::getParameter<settings::Integer>(parameters,"#idToLoad").value;
        std::dynamic_pointer_cast<sim::Morphology>(morphology)->set_morph_id(id);
        std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
    }else{
        morphology.reset(new sim::FixedMorphology(parameters));
        std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
        morphology->set_randNum(randNum);


        std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);;
    }


    //    float pos[3];
    //    simGetObjectPosition(std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMainHandle(),-1,pos);
}

void VisuInd::createController(){
    std::string fixed_morph_path =  settings::getParameter<settings::String>(parameters,"#robotPath").value;

    bool empty_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    if(empty_gen)
        return;

    int nn_type = settings::getParameter<settings::Integer>(parameters,"#NNType").value;
    int nb_hidden = settings::getParameter<settings::Integer>(parameters,"#NbrHiddenNeurones").value;
    const std::vector<int> joint_subs = settings::getParameter<settings::Sequence<int>>(parameters,"#jointSubs").value;
    int wheel_nbr,joint_nbr,sensor_nbr;
    if(fixed_morph_path == "None"){
        wheel_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_wheel_number();
        joint_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_joint_number();
        sensor_nbr = std::dynamic_pointer_cast<CPPNMorph>(morphology)->get_sensor_number();
    }else{
        wheel_nbr = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_wheelHandles().size();
        joint_nbr = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_jointHandles().size();
        sensor_nbr = std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->get_proxHandles().size();
    }

    bool use_ir = settings::getParameter<settings::Boolean>(parameters,"#useIR").value;
    bool use_camera = settings::getParameter<settings::Boolean>(parameters,"#useCamera").value;
    bool use_joint_feedback = settings::getParameter<settings::Boolean>(parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(parameters,"#useWheelFeedback").value;

    int nb_inputs = sensor_nbr + //proximity sensors
            (use_ir ? sensor_nbr : 0) + // IR sensors
            (use_joint_feedback ? joint_nbr : 0) + // joint positions
            (use_wheel_feedback ? wheel_nbr : 0) + // wheel positions
            (use_camera ? 1 : 0); // camera
    int nb_outputs = wheel_nbr + joint_nbr;

    int nbr_weights, nbr_bias;
    if(nn_type == settings::nnType::FFNN)
        NN2Control<ffnn_t>::nbr_parameters(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::RNN)
        NN2Control<rnn_t>::nbr_parameters(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN)
        NN2Control<elman_t>::nbr_parameters(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias);
    else if(nn_type == settings::nnType::ELMAN_CPG)
        NN2Control<elman_cpg_t>::nbr_parameters_cpg(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias,joint_subs);
    else if(nn_type == settings::nnType::CPG)
        NN2Control<cpg_t>::nbr_parameters_cpg(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias,joint_subs);
    else if(nn_type == settings::nnType::FF_CPG)
        NN2Control<ff_cpg_t>::nbr_parameters_cpg(nb_inputs,nb_hidden,nb_outputs,nbr_weights,nbr_bias,joint_subs);
    else {
        std::cerr << "unknown type of neural network" << std::endl;
        return;
    }

    std::cout << "number of weights : " << nbr_weights << " and number of biases : " << nbr_bias << std::endl;

    std::vector<double> weights = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_weights();
    std::vector<double> bias = std::dynamic_pointer_cast<NNParamGenome>(ctrlGenome)->get_biases();

    if(nn_type == settings::nnType::FFNN){
        control = std::make_shared<NN2Control<ffnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ffnn_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::ELMAN){
        control = std::make_shared<NN2Control<elman_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias);
    }
    else if(nn_type == settings::nnType::RNN){
        control = std::make_shared<NN2Control<rnn_t>>();
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<rnn_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias);
    }
    else if(nn_type == st::nnType::ELMAN_CPG){
        control.reset(new NN2Control<elman_cpg_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<elman_cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<elman_cpg_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias, joint_subs);
    }
    else if(nn_type == st::nnType::CPG){
        control.reset(new NN2Control<cpg_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<cpg_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias, joint_subs);
    }
    else if(nn_type == st::nnType::FF_CPG){
        control.reset(new NN2Control<ff_cpg_t>());
        control->set_parameters(parameters);
        std::dynamic_pointer_cast<NN2Control<ff_cpg_t>>(control)->set_randonNum(randNum);
        std::dynamic_pointer_cast<NN2Control<ff_cpg_t>>(control)->init_nn(nb_inputs,nb_hidden,nb_outputs,weights,bias, joint_subs);
    }
    else {
        std::cerr << "unknown type of neural network" << std::endl;
    }

}

void VisuInd::update(double delta_time){

    bool empty_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    if(empty_gen)
        return;

    bool use_joint_feedback = settings::getParameter<settings::Boolean>(parameters,"#useJointFeedback").value;
    bool use_wheel_feedback = settings::getParameter<settings::Boolean>(parameters,"#useWheelFeedback").value;

    double ctrl_freq = settings::getParameter<settings::Double>(parameters,"#ctrlUpdateFrequency").value;

    if( fabs(sum_ctrl_freq - ctrl_freq) < 0.0001){
        sum_ctrl_freq = 0;
        //- Retrieve sensors, joints and wheels values
        std::vector<double> inputs = morphology->update();

        if(use_joint_feedback){
            std::vector<double> joints = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_joints_positions();
            for(double &j: joints)
                j = 2.*j/M_PI;
            inputs.insert(inputs.end(),joints.begin(),joints.end());
        }
        if(use_wheel_feedback){
            std::vector<double> wheels = std::dynamic_pointer_cast<sim::Morphology>(morphology)->get_wheels_positions();
            inputs.insert(inputs.end(),wheels.begin(),wheels.end());
        }
        std::vector<double> outputs = control->update(inputs);
        morphology->command(outputs);
    }
    sum_ctrl_freq += settings::getParameter<settings::Float>(parameters,"#timeStep").value;
}

std::string VisuInd::to_string() const
{
    std::stringstream sstream;
    boost::archive::text_oarchive oarch(sstream);
    oarch.register_type<VisuInd>();
    oarch.register_type<NNParamGenome>();
    oarch.register_type<NN2CPPNGenome>();
    oarch.register_type<EmptyGenome>();
    oarch << *this;
    return sstream.str();
}

void VisuInd::from_string(const std::string &str){
    std::stringstream sstream;
    sstream << str;
    boost::archive::text_iarchive iarch(sstream);
    iarch.register_type<VisuInd>();
    iarch.register_type<NNParamGenome>();
    iarch.register_type<NN2CPPNGenome>();
    iarch.register_type<EmptyGenome>();
    iarch >> *this;

    //set the parameters and randNum of the genome because their are not included in the serialisation
    ctrlGenome->set_parameters(parameters);
    ctrlGenome->set_randNum(randNum);
    morphGenome->set_parameters(parameters);
    morphGenome->set_randNum(randNum);
}

void Visu::init(){
    int id = settings::getParameter<settings::Integer>(parameters,"#idToLoad").value;
    bool empty_ctrl_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    std::string fixed_morph_path =  settings::getParameter<settings::String>(parameters,"#robotPath").value;
    std::string manual_design = settings::getParameter<settings::String>(parameters,"#manualDesignFile").value;
    Genome::Ptr morph_gen;
    Genome::Ptr ctrl_gen;
    std::string ctrl_gen_file;
    std::string morph_gen_file;
    std::string cppn_file;
    std::string quadrics_file;

    if(manual_design != "None"){
        std::vector<std::vector<int>> list_of_voxel;
        sim::Morphology_CPPNMatrix::load_manual_design(manual_design,list_of_voxel);
        morph_gen.reset(new ManualDesign(list_of_voxel));
        ctrl_gen.reset(new EmptyGenome);
        Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
        return;
    }

    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    bool use_quadric = settings::getParameter<settings::Boolean>(parameters,"#useQuadric").value;

    if(!use_quadric){
        std::stringstream sstr2;
        sstr2 << folder_to_load << "/cppn_" << id;
        morph_gen_file = sstr2.str();
    }else{
        std::stringstream cppn_sstr;
        cppn_sstr << folder_to_load << "/cppn_" << id;
        cppn_file = cppn_sstr.str();
        std::string sq_file = settings::getParameter<settings::String>(parameters,"#quadricFile").value;
        quadrics_file = folder_to_load + std::string("/") + sq_file;
    }
    if(!empty_ctrl_gen){
        std::stringstream sstr;
        sstr << folder_to_load << "/ctrl_genome_" << id;
        ctrl_gen_file = sstr.str();
    }



    if(fixed_morph_path == "None"){
        //load morphology genome
        if(!use_quadric){
            nn2_cppn_t cppn;
            std::ifstream ifs(morph_gen_file);
            boost::archive::text_iarchive iarch(ifs);
            iarch >> cppn;
            morph_gen = std::make_shared<NN2CPPNGenome>(cppn);
            morph_gen->set_randNum(randomNum);
            morph_gen->set_parameters(parameters);
        }else{
            morph_gen = std::make_shared<SQCPPNGenome>(randomNum,parameters);
          //  std::dynamic_pointer_cast<SQCPPNGenome>(morph_gen)->load_from_files(cppn_file,quadrics_file);
        }
        morph_gen->set_id(id);
        if(empty_ctrl_gen)
            ctrl_gen.reset(new EmptyGenome);
        else{
            ctrl_gen.reset(new NNParamGenome(randomNum,parameters));
            std::dynamic_pointer_cast<NNParamGenome>(ctrl_gen)->from_file(ctrl_gen_file);
        }
        Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
        ind->set_parameters(parameters);
        ind->set_randNum(randomNum);
        population.push_back(ind);
    }else{
        morph_gen = std::make_shared<EmptyGenome>();
        if(empty_ctrl_gen){
            ctrl_gen = std::make_shared<EmptyGenome>();
            Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }else{
            ctrl_gen = std::make_shared<NNParamGenome>(randomNum,parameters);
            std::dynamic_pointer_cast<NNParamGenome>(ctrl_gen)->from_file(ctrl_gen_file);

            Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }
    }

    morph_gen.reset();
    ctrl_gen.reset();

    if(population.empty()){
        std::cerr << "ERROR: Population is empty" << std::endl;
        exit(1);
    }
}

bool Visu::update(const Environment::Ptr &env){
    Individual::Ptr ind = population[currentIndIndex];
    std::dynamic_pointer_cast<VisuInd>(ind)->set_trajectory(env->get_trajectory());
    return true;
}

bool Visu::is_finish(){
    return currentIndIndex >= population.size() - 1;
}


void Visu::load_per_id(int id, std::string &morph_gen_file, std::string &ctrl_gen_file){


}
