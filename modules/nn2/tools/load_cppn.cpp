#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "cppn.hpp"
#include <Eigen/Core>

std::mt19937 nn2::rgen_t::gen;

struct CPPN{

    nn2::default_cppn_t cppn;
    CPPN(){
        cppn = nn2::default_cppn_t();
    }


    template <class archive>
    void serialize(archive &arch, const unsigned int v)
    {
        arch & cppn;
    }
};

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "usage: path to cppn boost archive" << std::endl;
        return 1;
    }

    CPPN cppn;


    std::ifstream ifs(argv[1]);
    if(!ifs){
        std::cout << "unable to open " << argv[1] << std::endl;
        return 1;
    }
    boost::archive::text_iarchive iarch(ifs);
    iarch >> cppn;

    cppn.cppn.write_dot(std::cout);

    return 0;
}
