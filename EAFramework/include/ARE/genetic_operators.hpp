#ifndef GENETIC_OPERATORS_HPP
#define GENETIC_OPERATORS_HPP

#include "ARE/misc/RandNum.h"
#include <functional>

namespace are{

using mutators_t = std::function<void(std::vector<double>&, double* param, const misc::RandNum::Ptr&)>;

struct mutators{

    enum type{
        UNIFORM = 0,
        GAUSSIAN = 1,
        POLYNOMIAL = 2
    };

    /**
     * @brief uniform mutation operator
     *  Parameters :
     *      0 mutation rate
     *      1 upper bound
     *      2 lower bound
     */
    static mutators_t uniform;

    /**
     * @brief gaussian mutation operator.
     *  Parameters :
     *      0 mutation rate
     *      1 sigma
     *      2 upper bound
     *      3 lower bound
     */
    static mutators_t gaussian;

    /**
     * @brief polynomial mutation operator
     *  Parameters :
     *      0 mutation rate
     *      1 eta
     *      2 upper bound
     *      3 lower bound
     */
    static mutators_t polynomial;
};





using crossover_t = std::function<
                    void(const std::vector<double> &p1, const std::vector<double> &p2,
                    std::vector<double> &c1,std::vector<double>& c2,
            double* param, const misc::RandNum::Ptr &rn)>;

struct crossovers{
    enum type {
        NO_CROSSOVER = 0,
        RECOMBINATION = 1,
        SBX = 2
    };

    /**
     * @brief recombination
     */
    static crossover_t recombination;

    /**
     * @brief simulated binary crossover
     */
    static crossover_t sbx;
};

}//are

#endif //GENETIC_OPERATORS_HPP
