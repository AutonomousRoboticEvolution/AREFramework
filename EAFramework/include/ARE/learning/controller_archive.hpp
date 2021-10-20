#ifndef CONTROLLER_ARCHIVE_HPP
#define CONTROLLER_ARCHIVE_HPP

#include <vector>
#include "ARE/NNParamGenome.hpp"


namespace are {

typedef
/**
 * @brief Archive of controller ordered in a 3D vector with the following dimension : number of wheels, number of joints, number of sensors
 *      The archive store pairs of controller genome and associated fitness.
 */
struct ControllerArchive{
    typedef std::vector<std::vector<std::vector<std::pair<NNParamGenome::Ptr,double>>>> controller_archive_t;
    controller_archive_t archive;

    void init(int max_wheels,int max_joints, int max_sensors);

    /**
     * @brief update the archive. If the fitness of the candidate controller is greater than the stored one.
     * @param candidate new genome
     * @param associate fitness
     * @param number of wheels
     * @param number of joints
     * @param number of sensors
     */
    void update(const NNParamGenome::Ptr& genome, double fitness, int wheels, int joints, int sensors);

    /**
     * @brief reset all fitnesses at 0 while keeping the controllers genomes
     */
    void reset_fitnesses();

    std::string to_string() const;
    void from_file(const std::string&);

    template<class archive_t>
    void serialize(archive_t &arch, const unsigned int v)
    {
        arch & archive;
    }


} ControllerArchive;

}//are

#endif //CONTROLLER_ARCHIVE_HPP
