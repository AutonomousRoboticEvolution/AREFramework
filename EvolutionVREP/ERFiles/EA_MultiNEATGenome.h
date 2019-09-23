//
// Created by matteo on 9/23/19.
//

#ifndef ER_EA_MULTINEATGENOME_H
#define ER_EA_MULTINEATGENOME_H


#include "Genome.h"
#include "morphology/ER_VoxelInterpreter.h"
#include <multineat/Genome.h>

class EA_MultiNEATGenome : public Genome
{
public:
    explicit EA_MultiNEATGenome(NEAT::Genome &genome)
            : neat_genome(genome)
    {}

    virtual ~EA_MultiNEATGenome() = default;

    void init() override
    {
        auto morph = std::make_shared<ER_VoxelInterpreter>();
        NEAT::NeuralNetwork network;
        neat_genome.BuildPhenotype(network);
        morph->init(network);
        this->morph = morph;
    }

    void create() override
    {

    }

    void update() override
    {

    }

    std::shared_ptr<Genome> clone() const override
    {
        return std::shared_ptr<Genome>();
    }

    void mutate() override
    {

    }

    void savePhenotype(int indNum, int sceneNum) override
    {

    }

    bool loadGenome(int indNum, int sceneNum) override
    {
        return false;
    }

    bool loadGenome(std::istream &input, int indNum) override
    {
        return false;
    }

    void saveGenome(int indNum) override
    {

    }

    const std::string generateGenome() const override
    {
        return std::string();
    }

    void checkGenome() override
    {

    }

private:
    NEAT::Genome &neat_genome;
};


#endif //ER_EA_MULTINEATGENOME_H
