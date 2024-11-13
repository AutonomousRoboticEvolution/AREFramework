#include <iostream>
#include "simulatedER/nn2/NN2CPPNGenome.hpp"
#include <boost/archive/text_iarchive.hpp>

int main(int argc, char** argv)
{

    if(argc != 2){
        std::cerr << "usage: argument file containing a serialized cppn" << std::endl;
        return 1;
    }

    std::ifstream ifs(argv[1]);
    if(!ifs){
        std::cerr << "Unable to open file: " << argv[1] << std::endl;
        return 1;
    }
    boost::archive::text_iarchive iarch(ifs);
    are::NN2CPPNGenome cppn_gen;
    iarch >> cppn_gen;
    ifs.close();
    are::nn2_cppn_t cppn = cppn_gen.get_cppn();
    int i = 0;
    BGL_FORALL_VERTICES_T(v, cppn.get_graph(), are::nn2_cppn_t::graph_t){
                cppn.get_graph()[v]._id = boost::lexical_cast<std::string>(i++);
                std::cout << cppn.get_graph()[v]._id << " has a degree of " << degree(v,cppn.get_graph()) << std::endl;
    }

    std::ofstream ofs(std::string(argv[1]) + std::string(".dot"));
    if(!ofs)
    {
        std::cerr << "Unable to open file: " << argv[1] << ".dot" << std::endl;
        return 1;
    }
    cppn.write_dot(ofs);
    ofs.close();

    return 0;
}
