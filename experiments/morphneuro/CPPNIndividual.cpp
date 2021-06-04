#include "CPPNIndividual.h"
#include "simulatedER/mazeEnv.h"

using namespace are;
using namespace are::sim;

CPPNIndividual::CPPNIndividual(const Genome::Ptr& morph_gen,const Genome::Ptr& ctrl_gen) :
        Individual(morph_gen,ctrl_gen)
{
}

Individual::Ptr CPPNIndividual::clone()
{
    return std::make_shared<CPPNIndividual>(*this);
}

//TODO
void CPPNIndividual::update(double delta_time)
{
/* if(!morphology->stopSimulation){
        std::vector<double> inputs = morphology->update();
        double bias = 1.0;
        inputs.push_back(bias); //bias
        std::vector<double> outputs = control->update(inputs);
        std::vector<int> jointHandles =
                std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getJointHandles();

        assert(jointHandles.size() == outputs.size());

        for (size_t i = 0; i < outputs.size(); i++){
            simSetJointTargetVelocity(jointHandles[i],static_cast<float>(outputs[i]));  //Check leni's code
        }
    }
    else {
        simStopSimulation();
    }*/
    std::vector<double> inputs = morphology->update();
    double bias = 1.0;
    inputs.push_back(bias); //bias
    std::vector<double> outputs = control->update(inputs);
    morphology->command(outputs);
}

void CPPNIndividual::createMorphology()
{
    bool loadBestIndividual = settings::getParameter<settings::Boolean>(parameters,"#loadBestIndividual").value;
    bool loadSpecificIndividual = settings::getParameter<settings::Boolean>(parameters,"#loadSpecificIndividual").value;
    std::string show_best_ind_folder = settings::getParameter<are::settings::String>(parameters,"#showBestIndPath").value;
    std::vector<double> start_position;
    start_position = {settings::getParameter<settings::Double>(parameters,"#start_x").value,
                      settings::getParameter<settings::Double>(parameters,"#start_y").value,
                      settings::getParameter<settings::Double>(parameters,"#start_z").value};
    if(loadBestIndividual) {
        int bestIndividual = findBestIndividual(); // find the best individual
        //morphology->set_best_individual(bestIndividual);
        int indNum = bestIndividual;
        std::cout << "Loading genome " << indNum << "!" << std::endl;
        std::stringstream filepath;
        filepath << show_best_ind_folder << "/genome_morph" <<indNum;
        std::cout <<"---Loading Individual Genome File Path---" << std::endl;
        std::cout << filepath.str() << std::endl;

        NEAT::NeuralNetwork nn;
        //nn.Load(filepath.str().c_str());
        std::ifstream tt(filepath.str().c_str());
        NEAT::Genome neat_genome(tt); // load CPPN from the file
        morphology.reset(new Morphology_CPPNMatrix(parameters));
        neat_genome.BuildPhenotype(nn); //generate ANN
        std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
        std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(start_position[0],start_position[1],start_position[2]);
        setGenome();
        setMorphDesc();
        setManRes();

    }
    else if(loadSpecificIndividual) {
        int individualNum = settings::getParameter<settings::Integer>(parameters,"#individualNum").value;
        int indNum = individualNum;
        std::cout << "Loading genome " << indNum << "!" << std::endl;
        std::stringstream filepath;
        filepath << show_best_ind_folder << "/genome_morph" <<indNum;
        std::cout <<"---Loading Individual Genome File Path---" << std::endl;
        std::cout << filepath.str() << std::endl;
        NEAT::NeuralNetwork nn;
        //nn.Load(filepath.str().c_str());
        std::ifstream tt(filepath.str().c_str());
        NEAT::Genome neat_genome(tt); // load CPPN from the file
        morphology.reset(new Morphology_CPPNMatrix(parameters));
        neat_genome.BuildPhenotype(nn); //generate ANN
        std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
        std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(start_position[0],start_position[1],start_position[2]);
        setGenome();
        setMorphDesc();
        setManRes();
    }
    else {
        NEAT::Genome gen =
                std::dynamic_pointer_cast<CPPNGenome>(morphGenome)->get_neat_genome();
        morphology.reset(new Morphology_CPPNMatrix(parameters));
        NEAT::NeuralNetwork nn;
        gen.BuildPhenotype(nn);
        std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->setGenome(nn);
        std::dynamic_pointer_cast<sim::Morphology>(morphology)->createAtPosition(start_position[0],start_position[1],start_position[2]);  //fixed position
        setGenome();
        setMorphDesc();
        setManRes();
    }

}

void CPPNIndividual::createController()
{
    bool loadBestIndividual = settings::getParameter<settings::Boolean>(parameters,"#loadBestIndividual").value;
    bool loadSpecificIndividual = settings::getParameter<settings::Boolean>(parameters,"#loadSpecificIndividual").value;
    std::string show_best_ind_folder = settings::getParameter<are::settings::String>(parameters,"#showBestIndPath").value;
    if(loadBestIndividual){
        int bestIndividual = findBestIndividual(); // find the best individual
        std::stringstream directoryname;
        std::stringstream s;
        directoryname << show_best_ind_folder << "/genome_control";
        //int temp = mkdir(directoryname.str().c_str(), S_IRWXU);
        s << directoryname.str().c_str() << bestIndividual;
        std::cout <<"---Loading Individual Genome File Path---" << std::endl;
        std::cout << s.str() << std::endl;

        control.reset(new NNControl);
        control->set_parameters(parameters);

        NEAT::NeuralNetwork nn;
        NEAT::Substrate subs = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->get_substrate();  //location of sensors and actuators; when creating morphology, substrate is generated
        std::ifstream ifstream(s.str().c_str());
        NEAT::Genome neat_genome(ifstream); // load CPPN from the file
        neat_genome.BuildHyperNEATPhenotype(nn, subs); //generate ANN

        //nn.Load(s.str().c_str());

        std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
        std::cout << "after set nn" << std::endl;
    }else if(loadSpecificIndividual){
        int individualNum = settings::getParameter<settings::Integer>(parameters,"#individualNum").value;
        std::stringstream directoryname;
        std::stringstream s;
        directoryname << show_best_ind_folder << "/genome_control";
        //int temp = mkdir(directoryname.str().c_str(), S_IRWXU);
        s << directoryname.str().c_str() << individualNum;
        std::cout <<"---Loading Individual Genome File Path---" << std::endl;
        std::cout << s.str() << std::endl;

        control.reset(new NNControl);
        control->set_parameters(parameters);

        NEAT::NeuralNetwork nn;
        NEAT::Substrate subs = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->get_substrate();  //location of sensors and actuators
        std::ifstream ifstream(s.str().c_str());
        NEAT::Genome neat_genome(ifstream); // load CPPN from the file
        neat_genome.BuildHyperNEATPhenotype(nn, subs); //generate ANN

        //nn.Load(s.str().c_str());

        std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
        std::cout << "after set nn" << std::endl;
    }else{
        NEAT::Genome gen =
                std::dynamic_pointer_cast<CPPNGenome>(ctrlGenome)->get_neat_genome();
        NEAT::Substrate subs = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->get_substrate();
        control.reset(new NNControl);
        control->set_parameters(parameters);
        NEAT::NeuralNetwork nn;
        gen.BuildHyperNEATPhenotype(nn,subs);
        std::dynamic_pointer_cast<NNControl>(control)->nn = nn;
        //control.reset(new FixedController);
    }
}

void CPPNIndividual::setGenome()
{
    nn_morph = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getGenome();
}

void CPPNIndividual::setMorphDesc()
{
    morphDesc = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getMorphDesc();
}

void CPPNIndividual::setManRes()
{
    testRes = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getRobotManRes();
}

// Eigen::VectorXd CPPNIndividual::descriptor(){
//     return morphDesc;
// }

Eigen::VectorXd CPPNIndividual::descriptor(std::vector<double> final_position){
    double arena_size = settings::getParameter<settings::Double>(parameters,"#arenaSize").value;
    Eigen::Vector3d desc;
    desc << (final_position[0]+arena_size/2.)/arena_size, (final_position[1]+arena_size/2.)/arena_size, (final_position[2]+arena_size/2.)/arena_size;
    return desc;
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

/*
void CPPNIndividual::setManScore()
{
    manScore = std::dynamic_pointer_cast<Morphology_CPPNMatrix>(morphology)->getManScore();
}*/

int CPPNIndividual::findBestIndividual()
{
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;
    std::string fit_log_file = settings::getParameter<are::settings::String>(parameters,"#fitnessFile").value;
    std::string show_best_ind_folder = settings::getParameter<are::settings::String>(parameters,"#showBestIndPath").value;
    int seed = are::settings::getParameter<are::settings::Integer>(parameters,"#seed").value;
    std::vector<int> individuals;
    std::vector<float> fitnessValues;

    std::stringstream s;
    s << fit_log_file;
    std::ifstream file(show_best_ind_folder +'/' + s.str().c_str());
    std::cout << "---Fitness File Path---" << std::endl;
    std::cout << show_best_ind_folder +'/' + s.str().c_str() << std::endl;

    if (file.good()) {
        std::cout << "saved generations file found" << std::endl;
        std::string value;
        std::list<std::string> values;
        // read the settings file and store the comma seperated values
        while (file.good()) {
            getline(file, value, ','); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
            if (value.find('\n') != std::string::npos) {
                split_line(value, "\n", values);
            }
            else {
                values.push_back(value);
            }
        }
        file.close();

        std::list<std::string>::const_iterator it = values.begin();
        for (it = values.begin(); it != values.end(); it++) {
            std::string tmp = *it;
            if (tmp == "ind: ") {
                it++;
                tmp = *it;
                individuals.push_back(atoi(tmp.c_str()));
            }
            else if (tmp == "fitness: ") {
                it++;
                tmp = *it;
                fitnessValues.push_back(atof(tmp.c_str()));
            }
        }
    }
    int bestInd = 0;
    float bestFit = 0.0;

    for (int i = 0; i < individuals.size(); i++) {
        if (fitnessValues[i] >= bestFit) {
            bestFit = fitnessValues[i];
            bestInd = individuals[i];
        }
    }

    std::cout << "best individual " << bestInd << std::endl;
    return bestInd;
}

void CPPNIndividual::split_line(std::string& line, std::string delim, std::list<std::string>& values)
{
    size_t pos = 0;
    while ((pos = line.find(delim, (pos + 1))) != std::string::npos) {
        std::string p = line.substr(0, pos);
        values.push_back(p);
        line = line.substr(pos + 1);
    }

    if (!line.empty()) {
        values.push_back(line);
    }
}
