#pragma once
#include "ANN.h"

#include "NEAT_LIB\Assert.h"
#include "NEAT_LIB\Population.h"
#include "NEAT_LIB\Genome.h"

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

	shared_ptr<Control> clone() const;
	vector<float> update(vector<float> inputs);
};

