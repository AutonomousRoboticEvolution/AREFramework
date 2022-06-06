#ifndef CLIENT_ER_H
#define CLIENT_ER_H

#include "simulatedER/ER.h"
#include "ERClient/SlaveConnection.h"
#include "ERClient/sim_state.h"

#include <boost/archive/text_oarchive.hpp>


#include <boost/interprocess/managed_shared_memory.hpp>

extern "C" {
#if defined (VREP)
#include "v_repConst.h"
#elif defined (COPPELIASIM)
#include "simConst.h"
#endif
#include "remoteApi/extApiPlatform.h"
#include "remoteApi/extApi.h"
}

namespace are {

namespace client {

enum EvalOrder{
    FIFO = 0,
    FILO,
    RANDOM,
    FIFO_RANDOM
};


class ER : public sim::ER
{
public:
    ER():sim::ER(){}
    ~ER(){
    	for(auto& si : serverInstances)
		si.reset();
    }

    int init(int nbrOfInst,int port = 19997);
    void initialize();
    void startOfSimulation(int slaveIndex);
    void endOfSimulation(int slaveIndex);
    bool updateSimulation();
    bool execute();

    void quitSimulation();
    void finishConnections();
    void openConnections();
    void reopenConnections();
    bool confirmConnections();
    std::vector<std::unique_ptr<SlaveConnection>> updateSimulatorList();

private:
    std::vector<std::unique_ptr<SlaveConnection>> serverInstances;
    int population_size = 0;
    int loadingTrials = 3;
    bool sim_init = false;
    std::vector<Individual::Ptr> currentIndVec;
    std::vector<int> currentIndexVec;
    std::vector<int> indToEval;
    int nbrIndInEval = 0;
    std::vector<std::pair<hr_clock::time_point,hr_clock::time_point>> eval_times;
};

} //client
} //are
#endif //CLIENT_ER_H
