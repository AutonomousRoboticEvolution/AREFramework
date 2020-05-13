#ifndef SETTINGS_HPP
#define SETTINGS_HPP

namespace are {

namespace settings {

typedef enum nnType{
    FFNN = 0,
    RNN = 1,
    ELMAN = 2,
    CPG = 3
} nnType;

enum genomeType {
    NEAT = 0,
    NN = 1,
    NNPARAM = 2
};

typedef enum obsType {
    FINAL_POS = 0,
    TRAJECTORY = 1,
    POS_TRAJ = 2
}obsType;


}

#endif
