#include "physicalER/generate/ER_generate.hpp"

using namespace are::phy::generate;

void ER::initialize(){
    verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    int seed = settings::getParameter<settings::Integer>(parameters,"#seed").value;

    if(seed < 0){
        std::random_device rd;
        seed = rd();
        are::settings::random::parameters->emplace("#seed",new are::settings::Integer(seed));
    }
    randNum.reset(new are::misc::RandNum(seed));

    std::string exp_name = settings::getParameter<settings::String>(parameters,"#experimentName").value;
    std::string repository = settings::getParameter<settings::String>(parameters,"#repository").value;

    Logging::create_folder(repository + std::string("/") + exp_name);
    Logging::set_log_folder(repository + std::string("/") + exp_name);
    Logging::create_folder(repository + std::string("/") + exp_name + std::string("/waiting_to_be_built/"));
    Logging::create_folder(repository + std::string("/") + exp_name + std::string("/waiting_to_be_evaluated/"));

    if (verbose) {
        std::cout << "ER initialize" << std::endl;
    }

    std::string exp_plugin_name = settings::getParameter<settings::String>(parameters,"#expPluginName").value;
    std::vector<std::string> split_str;
    misc::split_line(exp_plugin_name,".",split_str);
    std::unique_ptr<dlibxx::handle> &libhandler = load_plugin(split_str[0] + "_generate.so");


    environment = std::make_shared<are::DummyEnv>(are::DummyEnv());
    environment->set_parameters(parameters);
    environment->set_randNum(randNum);

    if(!load_fct_exp_plugin<EA::Factory>
            (EAFactory,libhandler,"EAFactory"))
        exit(1);
    ea = EAFactory(randNum, parameters);
    ea->init();

    if(!load_fct_exp_plugin<Logging::Factory>
            (loggingFactory,libhandler,"loggingFactory"))
        exit(1);
    loggingFactory(logs,parameters);

    libhandler->close();

    if (verbose) std::cout << "ER initialized" << std::endl;
}

void ER::load_data(){
    ea->load_data_for_generate();
}

void ER::write_data(){
    ea->write_data_for_generate();
}

void ER::generate(){
    ea->epoch();
    ea->init_next_pop();
}

void ER::manufacturability_test(){
    if(settings::getParameter<settings::Boolean>(parameters,"#verbose").value)
        std::cout << "Starting Simulation" << std::endl;

    environment->init();
    currentInd = ea->getIndividual(currentIndIndex);
    currentInd->init();
    ea->setCurrentIndIndex(currentIndIndex);
    currentIndIndex++;

}


void ER::save_logs(bool endOfGen)
{
    for(const auto &log : logs){
        if(log->isEndOfGen() == endOfGen){
            log->saveLog(ea);
        }
    }
}
