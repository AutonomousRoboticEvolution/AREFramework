#ifndef CLIENT_ER_H
#define CLIENT_ER_H

#include "ARE/ER.h"
#include "ERClient/SlaveConnection.h"

#include <boost/archive/text_oarchive.hpp>


#include <boost/interprocess/managed_shared_memory.hpp>

extern "C" {
#include "v_repConst.h"
#include "remoteApi/extApiPlatform.h"
#include "remoteApi/extApi.h"
}


namespace are {

namespace client {

typedef enum simulation_state{
    IDLE = 0, READY = 1, BUSY = 2, FINISH = 3, ERROR = 9
} SimState;

class ER : public are::ER
{
public:
    ER(){}
    ~ER(){}

    int init(int nbrOfInst,int port = 19997);
    void initialize();
    void startOfSimulation();
    void endOfSimulation();
    void updateSimulation();
    bool execute();

    void quitSimulation();
    void finishConnections();
    void openConnections();
    void reopenConnections();
    bool confirmConnections();
private:
    std::vector<std::unique_ptr<SlaveConnection>> serverInstances;
    int loadingTrials = 1000;
    bool sim_init = false;

};

} //client
} //are
#endif //CLIENT_ER_H
