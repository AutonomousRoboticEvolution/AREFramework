#pragma once

#include "ANN.h"
#include "../../NEAT_LIB/NEAT_LIB/Assert.h"
#include "../../NEAT_LIB/NEAT_LIB/Population.h"
#include "../../NEAT_LIB/NEAT_LIB/Genome.h"


class CPPN :
	public ANN
{
public:
	CPPN();
	~CPPN();
	void init(int input, int inter, int output);
	void mutate(float mutationRate);
	void addNeurons(float mutationRate);
	void flush();

    std::shared_ptr<Control> clone() const;
    std::vector<float> update(std::vector<float> inputs);
};
