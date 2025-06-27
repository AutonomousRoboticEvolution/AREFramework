#include <iostream>
#include "client.hpp"
#include <signal.h>
#include <execinfo.h>

using namespace are;
using namespace are::sim;

/**
 * @brief signal handler to generate a stacktrace after getting a segfault signal.
 * @param sig
 */
void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char** argv){

    signal(SIGSEGV,handler);
    signal(SIGABRT,handler);
    signal(SIGILL,handler);
    signal(SIGFPE,handler);


    if(argc != 4)
    {
        std::cout << "usage :" << std::endl;
        std::cout << "\targ1 - path to the parameter file" << std::endl;
        std::cout << "\targ2 - port of the first simulator instance" << std::endl;
        std::cout << "\targ3 - number of simulator instances" << std::endl;
        return 1;
    }

    std::unique_ptr<Client> client = std::make_unique<Client>();
    std::vector<std::string> arguments(argv + 1, argv + argc);
    std::string parameters_file = argv[1];
    int port = atoi(argv[2]);
    int nbInst = atoi(argv[3]);

    settings::defaults::parameters->emplace("#evaluationOrder",std::make_shared<const settings::Integer>(1)); //Default first in last out

    //Load the parameters
    settings::ParametersMapPtr parameters = std::make_shared<settings::ParametersMap>(
    settings::loadParameters(parameters_file));
    // client->set_properties(std::make_shared<settings::Property>(settings::Property()));
    client->set_parameters(parameters);
    //-

    int seed = settings::getParameter<settings::Integer>(parameters,"#seed").value;
    if(seed < 0){
        std::random_device rd;
        seed = rd();
        settings::random::parameters->emplace("#seed",std::make_shared<const settings::Integer>(seed));
    }
    misc::RandNum rn(seed);
    client->set_rand_num(std::make_shared<misc::RandNum>(rn));


    bool verbose = settings::getParameter<settings::Boolean>(parameters,"#verbose").value;
    if(verbose)
    {
        std::cout << "arguments are: ";
        for (int i = 0; i < arguments.size(); i++) {
            std::cout << arguments[i] << ", ";
        }
        std::cout << std::endl;
    }

    if (!client->init(nbInst,port))
    {
        std::cerr << "Client unable to connect to the simulators instances !" << std::endl;
        return -1; // could not properly connect to servers
    }

        //Write parameters in the log folder.
        settings::saveParameters(are::Logging::log_folder + std::string("/parameters.csv"),parameters);

        // load or initialize EA
        // client->get_properties()->indCounter = 0;
     
        if (verbose) {
            std::cout << "initialized EA " << std::endl;
        }
    
        //Main Loop
        while (client->update_simulators()) usleep(5000);
    
        //client->quitSimulation();
        parameters.reset();
        client.reset();
        std::cout << "Client done, shutting down" << std::endl;
        return 0;
}
