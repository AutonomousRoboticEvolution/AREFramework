#pragma once
#include "ANN.h"
#include <vector>

class CustomANN :
	public ANN
{
public:
	CustomANN();
	~CustomANN();
	void init(int input, int inter, int output);
	vector<float> update(vector<float>);
	void mutate(float mutationRate); 
	void printNeuronValues(); 
	shared_ptr<Control> clone() const;
	int neuronID = 0; 
	int type = 0; 
	void checkConnections(); 
	stringstream getControlParams();
	void setControlParams(vector<string>);
	void changeConnectionIDToPointer(); 
	void cloneControlParameters(shared_ptr<Control> parent);
	int maxLayerSize = 2;
	int maxCon = 2;
	bool checkControl(vector<string> values);
	void changeConnectionPointerToID();
	void reset();
	
	void mutateConnections(float mutationRate);
	void addNeurons(float mutationRate);
	void setFloatParameters(vector<float> values);
	void removeNeurons(float mutationRate);
	void changeNeurons(float mutationRate);
//	float tmpMutRate = 0.05; //delete!!!

	/*struct Connection {
		float weight;
		float in; 
		float out; 
	};*/

//	vector<shared_ptr<Neuron>> allNeurons; 
	//vector<Connection> connections;
};

