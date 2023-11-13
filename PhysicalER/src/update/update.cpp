#include "physicalER/update/ER_update.hpp"

namespace are_set = are::settings;
namespace up = are::phy::update;

int main(int argc, char** argv){

    if(argc != 2){
        std::cout << "usage :" << std::endl;
        std::cout << "\targ1 - path to the parameter file" << std::endl;
    }

    up::ER::Ptr er = std::make_shared<up::ER>();
    are_set::ParametersMap param = are_set::loadParameters(argv[1]);
    er->set_parameters(std::make_shared<are_set::ParametersMap>(param));
    er->initialize();
    er->choice_of_robot();
    er->load_data();
    while(er->execute()) usleep(500);
    return 0;
}
