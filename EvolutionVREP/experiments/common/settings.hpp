#ifndef SETTINGS_HPP
#define SETTINGS_HPP

namespace are {

namespace settings {

typedef enum nn_type{
    FFNN = 0,
    CTRNN = 1,
    ELMAN = 2,
    CPG = 3
} nn_type;

enum genomeType {
    NEAT = 0,
    NN = 1,
    NNPARAM = 2
};

} 
}

#endif
