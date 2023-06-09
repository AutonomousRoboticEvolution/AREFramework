#ifndef NN2_SETTINGS_HPP
#define NN2_SETTINGS_HPP

namespace are {

namespace settings {

typedef enum nnType{
    FFNN = 0,
    RNN = 1,
    ELMAN = 2,
    ELMAN_CPG = 3,
    CPG = 4,
    FCP = 5,
    FF_CPG = 6
} nnType;

}
}

#endif
