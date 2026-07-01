#include "are_torch/torch_nn.hpp"
#include <cmath>
#include <random>

using namespace tnn;

/* Test of the CPGRBF network from the paper
 * Generic Neural Locomotion Control Framework for Legged Robots, M. Thor et al.
 * Should output several modulated oscilatory patterns.
 * The weights of the ouput layer are chosen randomly.
*/

int main(int argc, char** argv){
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> uniform(-1,1);

    std::vector<double> weights(40*4);
    std::vector<double> biases(4);
    for(int i = 0; i < 40*4; i++)
        weights[i] = uniform(gen);
    for(int i = 0; i < 4; i++)
        biases[i] = uniform(gen);


    CPGRBFNetwork cpgrbf(40,4);
    cpgrbf.init_cpg(1.1,0.01);
    cpgrbf.init_rbf();
    cpgrbf.set_out_layer_parameters(weights,biases);

    torch::Tensor x;
    for(int i = 0; i < 1000; i++){
        x = cpgrbf.forward();
        std::cout << x.data_ptr<double>()[0] << ","
                << x.data_ptr<double>()[1] << ","
                << x.data_ptr<double>()[2] << ","
                << x.data_ptr<double>()[3] << "," << std::endl;
    }

    return 0;
}
