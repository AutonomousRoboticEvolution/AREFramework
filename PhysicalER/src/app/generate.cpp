#include "physicalER/ER.hpp"

namespace are_set = are::settings;

int main(int argc, char** argv){

    if(argc != 2){
        std::cout << "usage :" << std::endl;
        std::cout << "\targ1 - path to the parameter file" << std::endl;
    }

    are::phy::ER::Ptr er(new are::phy::ER);
    are_set::ParametersMap param = are_set::loadParameters(argv[1]);
    er->set_parameters(std::make_shared<are_set::ParametersMap>(param));
    er->initialize();
    er->load_data(false);
    er->generate();
    er->write_data(false);
    er->save_logs();

    return 0;
}
