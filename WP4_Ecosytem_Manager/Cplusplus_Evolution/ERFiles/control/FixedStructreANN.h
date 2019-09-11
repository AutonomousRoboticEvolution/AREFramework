#pragma once
#include "ANN.h"
#include <vector>

class FixedStructureANN :
	public ANN
{
public:
    FixedStructureANN();
	~FixedStructureANN();
    /**
    @brief Initialize the NN
    @param input number of input neurons
    @param inter number of hidden neurons
    @param output number of output neurons
    */
	void init(int input, int inter, int output);
    /**
    @brief Update the NN
    @param vector<float> sensor input
    */
	std::vector<float> update(const std::vector<float> &sensorValues);
	void mutate(float mutationRate);
	std::shared_ptr<Control> clone() const;
	int neuronID = 0; 
	int type = 0;
    /// this function deletes the connections to deleted neurons and in turn the pointer to these neurons should go out of scope.
	void checkConnections(); 
	stringstream getControlParams();
	void setControlParams(const std::vector<std::string> &);
	void changeConnectionIDToPointer();
	bool checkControl(std::vector<std::string> values);
	/// destroy the neuron of CustomANN
	void reset();
	///reset the value of the neuron of CustomANN
	void flush();
	void setFloatParameters(std::vector<float> values);
};

