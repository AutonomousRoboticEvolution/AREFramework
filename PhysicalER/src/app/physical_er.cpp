#include "physicalER/ER.hpp"

int main(int argc, char** argv){

    if(argc != 2){
        std::cout << "usage :" << std::endl;
        std::cout << "\targ1 - path to the parameter file" << std::endl;
    }

    are::phy::ER::Ptr er;
    er->initialize();
    while(er->execute()) usleep(500);

    return 0;
}
