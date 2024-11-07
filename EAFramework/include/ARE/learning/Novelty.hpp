#ifndef NOVELTY_HPP
#define NOVELTY_HPP

#include "ARE/Individual.h"
#include <tbb/tbb.h>
#include <Eigen/Core>
#include "ARE/Logging.h"
#include <functional>

namespace are {

namespace novelty {
/**
 * @brief compute sparseness from a given list of distances and k_value a static parameter
 * @param a sorted list of distances.
 * @return sparseness
 */
template<typename Param>
double sparseness(const std::vector<double> &dist){
    double sum = 0;
    if(dist.size() >=  Param::k_value){
        for(int i = 0; i < Param::k_value; i++)
            sum += dist[i];
    }
    if(std::isnan(sum/static_cast<double>(Param::k_value))){
        std::cerr << "NaN found" << std::endl;
    }
    return sum/static_cast<double>(Param::k_value);
}

/**
 * @brief add ind to the archive if its novelty score is above a threshold or according a certain probability
 * @param individual
 * @param novelty score of the individual
 * @param archive
 * @param seed for the add archive probability
 */
template<typename Param>
void update_archive(const Eigen::VectorXd& ind_desc,
                           double ind_nov,
                           std::vector<Eigen::VectorXd> &archive,
                    const misc::RandNum::Ptr &rn){

    if(ind_nov > Param::novelty_thr || rn->randFloat(0,1) < Param::archive_adding_prob){
         archive.push_back(ind_desc);
     }
}
}
struct Novelty {

    typedef std::function<double(Eigen::VectorXd,Eigen::VectorXd)> distance_fct_t;

    typedef struct distance_fcts{
        static distance_fct_t euclidian;
        static distance_fct_t positional;
        static distance_fct_t positional_normalized;
    } distance_fcts;

    /**
     * @brief Compute distances of a descriptor to the archive and a population. And return the vector of distances sorted in increasing order.
     * @param descriptor
     * @param archive of descriptor
     * @param population
     * @return vector of distances
     */
    static std::vector<double> distances(const Eigen::VectorXd& desc,
                                        const std::vector<Eigen::VectorXd> &archive,
                                        const std::vector<Eigen::VectorXd> &pop,
                                        const distance_fct_t dist_fct = distance_fcts::euclidian);

    /**
     * @brief Compute distances of a descriptor to the archive and a population. And return the vector of distances sorted in increasing order.
     * @param descriptor
     * @param archive of descriptor
     * @param population
     * @param output : sorted indexes of the population from the closest to the farthest of the desc
     * @return vector of distances
     */
    static std::vector<double> distances(const Eigen::VectorXd& desc,
                                        const std::vector<Eigen::VectorXd> &archive,
                                        const std::vector<Eigen::VectorXd> &pop,
                                        std::vector<size_t> & sorted_pop_indexes,
                                         const distance_fct_t dist_fct = distance_fcts::euclidian);

    struct default_params{
        static int k_value;
        static double novelty_thr;
        static double archive_adding_prob;
    };



};

template <class ea_t>
class ArchiveLog : public Logging
{
public:
    ArchiveLog(const std::string &file) : Logging(file,true){} //Logging at the end of the generation
    void saveLog(EA::Ptr & ea){
        int generation = ea->get_generation();

        std::ofstream savePopFile;
        if(!openOLogFile(savePopFile))
            return;

        Eigen::VectorXd desc;

        savePopFile << generation << ";" << static_cast<ea_t*>(ea.get())->get_archive().size() << ";";
        for (size_t i = 0; i < static_cast<ea_t*>(ea.get())->get_archive().size(); i++) {
            desc = static_cast<ea_t*>(ea.get())->get_archive()[i];
            for(size_t j = 0; j < desc.rows() - 1 ; j++)
                savePopFile << desc(j) << ",";
            savePopFile << desc(desc.rows()-1) << ";";
        }
        savePopFile << std::endl;
        savePopFile.close();
    }
    void loadLog(const std::string& logFile){}
};


}

#endif //NOVELTY_HPP
