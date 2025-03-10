#ifndef RANDOM_HPP
#define RANDOM_HPP

namespace nn2 {

struct random_generator{
    static std::mt19937 gen;
//    static void seed(int s){gen.seed(s);}
};

using rgen_t = random_generator;

}

#endif //RANDOM_HPP
