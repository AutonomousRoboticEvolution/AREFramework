#include "are_torch/torch_nn.hpp"
#include <cmath>
using namespace tnn;

/* Test of the RBF network with gaussian kernel from
 * Generic Neural Locomotion Control Framework for Legged Robots, M. Thor et al.
 * The parameters are hand tuned to roughly approximate the sinusoidal function.
 *
*/

int main(int argc, char** argv){
    std::vector<double> data;
    for(double x = 0; x < 10; x+=0.01)
        data.push_back(std::sin(x));


    std::vector<double> centers, variances, weights;
    for(double x = 0; x < 10; x+=0.25){
        centers.push_back(std::sin(x));
        variances.push_back(1);
        weights.push_back(std::sin(x) >= 0 ? 0.04 : -0.1);
    }


    RBFNetwork rbf(1,40,1);
    torch::Tensor x = torch::tensor({1,2});
    rbf.set_centers(centers);
    rbf.set_variances(variances);
    rbf.set_weights_biases(weights,{0});
    for(const double& x: data){
        std::cout << x << "," << rbf.forward(torch::tensor({x})).data_ptr<double>()[0] << std::endl;
    }


    return 0;
}
