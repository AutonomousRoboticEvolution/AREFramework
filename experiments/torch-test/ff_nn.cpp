#include "ff_nn.hpp"

torch::Tensor FeedForwardNN::forward(torch::Tensor x){
    x = _linear1(x);
    std::cout << "linear combination i->h : " << x << std::endl;
    x = _activation(x);
    std::cout << "sigmoid activation h : " << x << std::endl;
//    x = _linear2(x);
//    std::cout << "linear combination h->o : " << x << std::endl;
//    x = _activation(x);
//    std::cout << "sigmoid activation o : " << x << std::endl;
    return x;
}

int main( int argc, char** argv) {
    if(argc < 3)
        exit(1);

    FeedForwardNN ff_nn;

    torch::Tensor x = torch::tensor(std::vector({std::stod(argv[1]),std::stod(argv[2])}));
    torch::Tensor y;
    for(int i = 0; i < 3; i++){
         y = ff_nn.forward(x);
         std::cout << "output : " << y << std::endl;
         for(const auto& param : ff_nn.parameters()){
             std::cout << param << std::endl;
         }
         ff_nn._linear1->weight = ff_nn.parameters()[0];
         ff_nn._linear1->bias = ff_nn.parameters()[1];
    }


    return 0;
}
