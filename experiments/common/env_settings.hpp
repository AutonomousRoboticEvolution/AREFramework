#ifndef ENV_SETTINGS_HPP
#define ENV_SETTINGS_HPP

#include <iostream>
#include <string>

namespace are {

namespace sim {

typedef enum task_t{
    MAZE = 0,
    OBSTACLES = 1,
    MULTI_TARGETS = 2,
    EXPLORATION = 3,
    BARREL = 4,
    GRADUAL = 5,
    LOCOMOTION = 6,
    HILL_CLIMBING = 7,
    PUSH_OBJECT = 8
} task_t;

inline std::string task_name(are::sim::task_t task){
    if(task == sim::MAZE)
        return "maze";
    else if(task == sim::OBSTACLES)
        return "obstacles";
    else if(task == sim::MULTI_TARGETS)
        return "multi targets";
    else if(task == sim::EXPLORATION)
        return "exploration";
    else if(task == sim::BARREL)
        return "barrel";
    else if(task == sim::GRADUAL)
        return "gradual";
    else if(task == sim::LOCOMOTION)
        return "locomotion";
    else if(task == sim::HILL_CLIMBING)
        return "hill_climbing";
    else if(task == sim::PUSH_OBJECT)
        return "push_object";
    else{
        std::cerr << "Error: task unknow" << std::endl;
        return "None";
    }
}

}//sim

}//are



#endif //ENV_SETTINGS_HPP
