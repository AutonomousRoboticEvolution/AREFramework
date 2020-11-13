//
// Created by matteo on 9/23/19.
//

#ifndef ER_EA_MULTINEATGENOME_H
#define ER_EA_MULTINEATGENOME_H


#include "ARE/Genome.h"
#include "ARE/ER_VoxelInterpreter.h"
#include <multineat/Genome.h>

namespace are {

class EA_MultiNEATGenome : public Genome
{
public:
    explicit EA_MultiNEATGenome(NEAT::Genome &genome)
            : neat_genome(genome)
    {
        //std::cout << "Created " << __func__ << " with id:" << genome.GetID() << std::endl;
    }

    virtual ~EA_MultiNEATGenome() = default;

    virtual Phenotype::Ptr develop(){
        ER_VoxelInterpreter::Ptr morph = std::make_shared<ER_VoxelInterpreter>(neat_genome.GetID());
        NEAT::NeuralNetwork network;
        neat_genome.BuildPhenotype(network);
        morph->init(network);
        return morph;
    }

    const std::string generateGenome() const override
    {
        return std::string();
    }

private:
    NEAT::Genome &neat_genome;
};

}//are

#endif //ER_EA_MULTINEATGENOME_H
