#pragma once
#include "ANN.h"
class CPPN :
	public ANN
{
public:
	CPPN();
	~CPPN();
	void init(int input, int inter, int output);
	void addNeurons(float mutationRate);
	shared_ptr<Control> clone();
	vector<float> update(vector<float> inputs);
};

