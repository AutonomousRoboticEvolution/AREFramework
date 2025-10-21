#include "are_torch/torch_nn.hpp"
#include <cmath>
using namespace tnn;

/* Test of the CPG network.
 * The parameters are from "SO(2)-Networks as Neural Oscillators", F. Pasemann, 2003
 * The output should be two sine waves
 */

int main(int argc, char** argv){
    if(argc < 4){
        std::cerr << "Usage: ./cpg_test alpha phi init_vals" << std::endl;
        return -1;
    }
    double alpha = atof(argv[1]);
    double phi = atof(argv[2]);
    double init_vals = atof(argv[3]);
    std::vector<double> weights = {alpha*cos(phi*M_PI_2),alpha*sin(phi*M_PI_2),
                                   -alpha*sin(phi*M_PI_2),alpha*cos(phi*M_PI_2)};
    std::vector<double> biases(2,0);

    CPGCell cpg(init_vals,-init_vals);
    cpg->set_weights_biases(weights,biases);
    torch::Tensor x;
    for(int i = 0; i < 1000; i++){
        x = cpg->forward();
        std::cout << x.data_ptr<double>()[0] << "," << x.data_ptr<double>()[1] << std::endl;
    }
    return 0;
}
