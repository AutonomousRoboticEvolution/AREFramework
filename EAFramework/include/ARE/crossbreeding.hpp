#ifndef CROSSBREEDING_HPP
#define CROSSBREEDING_HPP

#include "functional"
#include "ARE/Settings.h"
#include "ARE/misc/RandNum.h"
#include "ARE/io_helpers.hpp"
#include "ARE/nn2/NN2CPPNGenome.hpp"
#include "ARE/learning/Novelty.hpp"

namespace are {

class Crossbreeding{
public:

    /**
     * @brief selection_fct_t (number of genomes to select, list of genomes to select from) -> list of selected ids
     */
    typedef std::function<std::vector<int>(int, const ioh::MorphGenomeInfoMap&)> selection_fct_t;
    typedef struct selection_fct{
        static selection_fct_t best_fitness; //select based on best fitness
    } selection_fct;

    /**
     * @brief trigger_criterion_t (list of genomes to analyse) -> true or false, whether to trigger the crossbreeding
     */
    typedef std::function<bool(const ioh::MorphGenomeInfoMap&,const settings::ParametersMapPtr&)> trigger_criterion_t;
    typedef struct trigger_criterion{
        static trigger_criterion_t never; //always return false
        static trigger_criterion_t always; //always return true
        static trigger_criterion_t manual; //manual triggering
        static trigger_criterion_t low_fitness; //based on fitnesses lower to a certain threshold
        static trigger_criterion_t low_diversity; //based to a diversity lower to a certain threshold
    } trigger_criterion;


    Crossbreeding(const settings::ParametersMapPtr& param, const misc::RandNum::Ptr& rdnbr):
        parameters(param), rand_nbr(rdnbr),
        _selection_fct(selection_fct::best_fitness),_trigger(trigger_criterion::never){}

    Crossbreeding(const Crossbreeding& cb):
        parameters(cb.parameters),
        rand_nbr(cb.rand_nbr),
        _selection_fct(cb._selection_fct),
        _trigger(cb._trigger){}

    /**
     * @brief based on a list of robot return true if crossbreeding should be done, false otherwise
     * @return
     */
    bool should_crossbreed(const ioh::MorphGenomeInfoMap& mgim);

    void selection(int nbr_to_select, std::map<int, NN2CPPNGenome::Ptr> &genomes);

    void set_selection_fct(selection_fct_t fct){_selection_fct = fct;}
    void set_trigger(trigger_criterion_t trigger){_trigger = trigger;}

private:
    settings::ParametersMapPtr parameters;
    misc::RandNum::Ptr rand_nbr;

    selection_fct_t _selection_fct;
    trigger_criterion_t _trigger;
};
}//are
#endif //CROSSBREEDING_HPP
