#ifndef RANDOM_SEARCH_HPP
#define RANDOM_SEARCH_HPP

#include <limbo/init/lhs.hpp>
#include "ARE/EA.h"
#include "simulatedER/nn2/NN2Individual.hpp"
#include "simulatedER/mazeEnv.h"
#include "ARE/nn2/NN2Settings.hpp"
#include "ARE/Settings.h"


namespace are
{

class RandomSearch : public EA
{
public:
    RandomSearch() : EA(){}
    RandomSearch(const misc::RandNum::Ptr& rn, const settings::ParametersMapPtr& param) : EA(rn, param){}
    ~RandomSearch(){}

    void init();
    bool update(const Environment::Ptr&);

    void setObjectives(size_t indIdx, const std::vector<double> &objectives);

    bool is_finish();

protected:
    size_t currentIndIndex;
    bool _is_finish = false;
};





}

#endif //RANDOM_SEARCH_HPP
