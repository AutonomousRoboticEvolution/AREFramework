#ifndef SIM_STATE_H
#define SIM_STATE_H

namespace are
{
namespace client
{

typedef enum simulation_state{
    IDLE = 0, READY = 1, BUSY = 2, FINISH = 3, RESTART = 4, ERROR = 9
} SimState;

}
}

#endif
