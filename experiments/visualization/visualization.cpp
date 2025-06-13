#include "visualization.hpp"


using namespace are;
namespace  fs = boost::filesystem;
namespace st = settings;

void VisuInd::createMorphology(){
    std::string fixed_morph_path =  settings::getParameter<settings::String>(parameters,"#robotPath").value;
    std::vector<double> init_pos = settings::getParameter<settings::Sequence<double>>(parameters,"#initPosition").value;
    std::string manual_design = settings::getParameter<settings::String>(parameters,"#manualDesignFile").value;

    if(manual_design != "None"){
        morphology.reset(new CPPNMorph(parameters));
        morphology->set_randNum(randNum);
        std::dynamic_pointer_cast<CPPNMorph>(morphology)->set_list_of_voxels(std::dynamic_pointer_cast<ManualDesign>(morphGenome)->list_of_voxels);
        std::dynamic_pointer_cast<CPPNMorph>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
        return;
    }
    sim::NN2Individual::createMorphology();

    // if(fixed_morph_path == "None"){

    //     int id = settings::getParameter<settings::Integer>(parameters,"#idToLoad").value;

    //     morphology.reset(new CPPNMorph(parameters));
    //     morphology->set_randNum(randNum);
    //     std::dynamic_pointer_cast<sim::Morphology>(morphology)->set_morph_id(id);


    //     nn2_cppn_t gen = std::dynamic_pointer_cast<NN2CPPNGenome>(morphGenome)->get_cppn();
    //     std::dynamic_pointer_cast<CPPNMorph>(morphology)->setNN2CPPN(gen);

    //     std::dynamic_pointer_cast<CPPNMorph>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);
    // }else{
    //     morphology.reset(new sim::FixedMorphology(parameters));
    //     std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->loadModel();
    //     morphology->set_randNum(randNum);


    //     std::dynamic_pointer_cast<sim::FixedMorphology>(morphology)->createAtPosition(init_pos[0],init_pos[1],init_pos[2]);;
    // }


    //    float pos[3];
    //    simGetObjectPosition(std::dynamic_pointer_cast<CPPNMorph>(morphology)->getMainHandle(),-1,pos);
}

void VisuInd::createController(){

    bool empty_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    if(empty_gen)
        return;

    sim::NN2Individual::createController();
}

void VisuInd::update(double delta_time){

    bool empty_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    if(empty_gen)
        return;

    Individual::update(delta_time);
    float time_step = settings::getParameter<settings::Float>(parameters,"#timeStep").value;
    if(fabs(sum_ctrl_freq - time_step) < 0.0001 && !rollout.empty())
        std::cout << rollout.back().to_string() << std::endl;
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
    generation = settings::getParameter<settings::Integer>(parameters,"#genToLoad").value;
    int indIdx = settings::getParameter<settings::Integer>(parameters,"#indToLoad").value;
    bool empty_ctrl_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    std::string fixed_morph_path =  settings::getParameter<settings::String>(parameters,"#robotPath").value;
    std::string manual_design = settings::getParameter<settings::String>(parameters,"#manualDesignFile").value;
    Genome::Ptr morph_gen;
    Genome::Ptr ctrl_gen;
    std::vector<std::string> ctrl_gen_files;
    std::vector<std::string> morph_gen_files;

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

    if(id >= 0)
        load_per_id(id,morph_gen_files,ctrl_gen_files);
    else load_per_gen_ind(indIdx,morph_gen_files,ctrl_gen_files);

    if(fixed_morph_path == "None"){
        //load morphology genome
        for(size_t i = 0; i < morph_gen_files.size(); i++){

            nn2_cppn_t cppn;
            std::ifstream ifs(morph_gen_files[i]);
            boost::archive::text_iarchive iarch(ifs);
            iarch >> cppn;
            morph_gen.reset(new NN2CPPNGenome(cppn));
            std::vector<std::string> split_str;
            misc::split_line(morph_gen_files[i],"_",split_str);
            morph_gen->set_id(std::stoi(split_str.back()));

            morph_gen->set_randNum(randomNum);
            morph_gen->set_parameters(parameters);


            if(empty_ctrl_gen)
                ctrl_gen.reset(new EmptyGenome);
            else{
                ctrl_gen.reset(new NNParamGenome(randomNum,parameters));
                std::dynamic_pointer_cast<NNParamGenome>(ctrl_gen)->from_file(ctrl_gen_files[i]);
            }

            Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }

    }else{
        morph_gen = std::make_shared<EmptyGenome>();
        if(empty_ctrl_gen){
            ctrl_gen = std::make_shared<EmptyGenome>();
            Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
            ind->set_parameters(parameters);
            ind->set_randNum(randomNum);
            population.push_back(ind);
        }else{
            for(const std::string& path: ctrl_gen_files){
                std::cout << "ctrl file loading: " << path << std::endl;
                ctrl_gen = std::make_shared<NNParamGenome>(randomNum,parameters);
                std::dynamic_pointer_cast<NNParamGenome>(ctrl_gen)->from_file(path);

                Individual::Ptr ind(new VisuInd(morph_gen,ctrl_gen));
                ind->set_parameters(parameters);
                ind->set_randNum(randomNum);
                population.push_back(ind);
            }
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
}

bool Visu::is_finish(){
    return currentIndIndex >= population.size() - 1;
}

void Visu::load_per_gen_ind(int indIdx, std::vector<std::string>& morph_gen_files, std::vector<std::string>& ctrl_gen_files){
    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    bool empty_ctrl_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    bool load_all_morph_gen = settings::getParameter<settings::Boolean>(parameters,"#loadAllMorphGenome").value;

    if(!empty_ctrl_gen){
        if(indIdx < 0){
            int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
            for(int i = 0; i < pop_size; i++){
                std::stringstream sstr2;
                sstr2 << folder_to_load << "/ctrlGenome_" << generation << "_" << i;
                ctrl_gen_files.push_back(sstr2.str());
            }
        }
        else{
            std::stringstream sstr;
            sstr << folder_to_load << "/ctrlGenome_" << generation << "_" << indIdx;
            ctrl_gen_files.push_back(sstr.str());
        }
    }

    if(load_all_morph_gen){
        std::string filename;
        std::vector<std::string> split_str;
        for(const auto &dirit : fs::directory_iterator(fs::path(folder_to_load))){
            filename = dirit.path().string();
            boost::split(split_str,filename,boost::is_any_of("/"));
            boost::split(split_str,split_str.back(),boost::is_any_of("G"));
            if(split_str[0] == "morph")
                morph_gen_files.push_back(filename);
        }
    }else{
        if(indIdx < 0){
            int pop_size = settings::getParameter<settings::Integer>(parameters,"#populationSize").value;
            for(int i = 0; i < pop_size; i++){
                std::stringstream sstr2;
                sstr2 << folder_to_load << "/morphGenome_" << generation << "_" << i;
                morph_gen_files.push_back(sstr2.str());
            }
        }else{
            std::stringstream sstr2;
            sstr2 << folder_to_load << "/morphGenome_" << generation << "_" << indIdx;
            morph_gen_files.push_back(sstr2.str());
        }
    }
}

void Visu::load_per_id(int id, std::vector<std::string> &morph_gen_files, std::vector<std::string> &ctrl_gen_files){
    std::string folder_to_load = settings::getParameter<settings::String>(parameters,"#folderToLoad").value;
    bool empty_ctrl_gen = settings::getParameter<settings::Boolean>(parameters,"#emptyCtrlGenome").value;
    bool load_all_morph_gen = settings::getParameter<settings::Boolean>(parameters,"#loadAllMorphGenome").value;

    if(load_all_morph_gen){
        std::string filename;
        std::vector<std::string> split_str;
        for(const auto &dirit : fs::directory_iterator(fs::path(folder_to_load))){
            filename = dirit.path().string();
            boost::split(split_str,filename,boost::is_any_of("/"));
            boost::split(split_str,split_str.back(),boost::is_any_of("_"));
            if(split_str[0] == "morph" && split_str[1] == "genome")
                morph_gen_files.push_back(filename);
            if(!empty_ctrl_gen){
                std::stringstream sstr;
                sstr << folder_to_load << "/ctrl_genome_" << split_str.back();
                ctrl_gen_files.push_back(sstr.str());
            }
        }
    }else{
        std::stringstream sstr2;
        sstr2 << folder_to_load << "/morph_genome_" << id;
        morph_gen_files.push_back(sstr2.str());
        if(!empty_ctrl_gen){
            std::stringstream sstr;
            sstr << folder_to_load << "/ctrl_genome_" << id;
            ctrl_gen_files.push_back(sstr.str());

        }
    }
}
