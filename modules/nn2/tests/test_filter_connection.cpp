#include "test_filter_connection.hpp"
#include <cmath>

int main(int argc,char** argv){

    FilterPerceptron<nn2::ffcpg::neuron_t,nn2::ffcpg::connection_t> fp;
    fp.set_all_weights({1,1});
    fp.init();
    for(int i = -1000; i < 1000; i++){
        std::vector<double> inputs = {std::sin(static_cast<double>(i)*0.1),static_cast<double>(i)*0.001f};
        fp.step(inputs,0);
        std::cout << inputs[0] << ";" << inputs[1] << ";" << fp.outf()[0] << std::endl;
    }

    return 0;
}

