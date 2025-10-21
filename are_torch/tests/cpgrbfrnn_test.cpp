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

    //define linear input
    std::vector<double> data;
    for(double x = 0; x < 1; x+=0.001)
        data.push_back(x);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> uniform(-1,1);

    std::vector<double> weights(40*4);
    for(int i = 0; i < 40*4; i++)
        weights[i] = uniform(gen);

    CPGRBFRNN cpgrbfrnn(1,40,6,4);
    cpgrbfrnn.init_cpg(1.05,0.1);
    cpgrbfrnn.init_rbf();
    cpgrbfrnn.set_out_layer_parameters(weights,{0,0,0,0});

    torch::Tensor x;
    for(double in: data){
        x = cpgrbfrnn.forward(torch::tensor({in},torch::TensorOptions().dtype(torch::kDouble)));
        std::cout << x.data_ptr<double>()[0] << ","
                << x.data_ptr<double>()[1] << ","
                << x.data_ptr<double>()[2] << ","
                << x.data_ptr<double>()[3] << "," << std::endl;
    }

    return 0;
}
