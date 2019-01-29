#include "PatternNeuron.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>


PatternNeuron::PatternNeuron() {
	reset();
}

PatternNeuron::~PatternNeuron() {
	//	cout << "DELETED CONTROL" << endl; 
}

void PatternNeuron::setFloatParameters(vector<float> values) {
	// function can be used to manually set specific parameters
	frequency = values[0];
	amplitude = values[1]; 
	phase = values[2];
}

void PatternNeuron::init(int id) {
	neuronID = id;
	amplitude = randomNum->randFloat(-1.0, 1.0);
	frequency = randomNum->randFloat(-1.0, 1.0);
	phase = randomNum->randFloat(-1.0, 1.0);
}
void PatternNeuron::update() {
//	if (usePhaseControl && !useAngularFreqControl && !useAmplitudeControl) {
//		targetPosition = (float)(maxAmplitude[module] * Math.sin(maxAngFreq[module] * time + phaseControl[module] / 180 * Math.M_PI));
//	}
//	else {
//		double amplitude = maxAmplitude[module] * amplitudeControl[module];
//		double angularFreq = maxAngFreq[module] * andularFreqControl[module];
//		targetPosition = (float)(amplitude * Math.sin(angularFreq * time + phaseControl[module] / 180 * Math.M_PI));
//	}
	// triangle
//	float a = frequency * time;
//	output = (float) sinf(a);
	// sin
	double amplitudeD = amplitude;
	double angularFreq = frequency * maxFreq;
	// angularFreq = 3.0;
	// amplitudeD = 1.0;
	//phase = 1.0;
	output = (float)(amplitudeD * sin(angularFreq * time + (phase * M_PI))); //; / 180 * M_PI));
	if (output > 1.0) {
		output = 1.0;
	}
	else if (output < -1.0) {
		output = -1.0;
	}
//	cout << "output = " << output << endl;
//	cout << "time: " << time << endl;
//	cout << "amplitude: " << amplitude << endl;
//	cout << "a-frequency: " << angularFreq << endl;

//	output = (float)amplitude *(sin((frequency * time * M_PI) + (phase * 2 * M_PI)));
	time += timeStep;
	

	//	cout << "innerValue = " << innerValue << endl; 
//	output = sqrt((cycleRange * cycleRange) - (innerValue * innerValue));
	for (int i = 0; i < connections.size(); i++) {
//		connections[i]->input += output * connectionWeights[i];
		connections[i]->input = output * connectionWeights[i];
		//		cout << "con weight = " << connectionWeights[i] << endl;
	}
	input = 0;
//	cout << "innerValue " << neuronID << " = " << innerValue << endl; 
}

void PatternNeuron::reset() {
	input = 0.0;
	output = 0.0;
	time = 0.0;
}

shared_ptr<Neuron> PatternNeuron::clone() {
	shared_ptr<PatternNeuron> thisNeuron = make_unique<PatternNeuron>(*this);
	thisNeuron->input = 0;
	thisNeuron->output = 0;
	return thisNeuron;
}

void PatternNeuron::mutate(float mutationRate) {
	if (mutationRate > randomNum->randFloat(0.0, 1.0)) {
		if (randomNum->randFloat(0.0, 1.0) > 0.5) {
			frequency = randomNum->randFloat(-1.0, 1.0);
		} 
		else {
			frequency += randomNum->randFloat(-sigma, sigma);
			if (frequency > 1.0) {
				frequency = 1.0;
			}
			else if (frequency < -1.0) {
				frequency = -1.0;
			}
		}
	}
	if (mutationRate > randomNum->randFloat(0.0, 1.0)) {
		if (randomNum->randFloat(0.0, 1.0) > 0.5) {
			amplitude = randomNum->randFloat(-1.0, 1.0);
		}
		else {
			amplitude += randomNum->randFloat(-sigma, sigma);
			if (amplitude > 1.0) {
				amplitude = 1.0;
			}
			else if (amplitude < -1.0) {
				amplitude = -1.0;
			}
		}
	}	
	if (mutationRate > randomNum->randFloat(0.0, 1.0)) {
		if (randomNum->randFloat(0.0, 1.0) > 0.5) {
			phase = randomNum->randFloat(-1.0, 1.0);
		}
		else {
			phase += randomNum->randFloat(-sigma, sigma);
			if (phase > 1.0) {
				phase = 1.0;
			}
			else if (phase < -1.0) {
				phase = -1.0;
			}
		}
	}
	if (settings->controlType != settings->ANN_CUSTOM) {
		Neuron::mutate(mutationRate);
	}
}

stringstream PatternNeuron::getNeuronParams() {
	stringstream ss; 
	ss << "5," << endl; // 5 = PatternNeuron
	ss << Neuron::getNeuronParams().str(); 
	ss << "\t" << ",#Frequency," << to_string(frequency) << "," << endl;
	ss << "\t" << ",#Amplitude," << to_string(amplitude) << "," << endl;
	ss << "\t" << ",#Phase," << to_string(phase) << "," << endl;
//	ss << "\t" << "#time: " << time << endl;
//	ss << "\t" << "#output: " << output << endl;
//	ss << "\t" << "#input: " << input << endl;
	ss << "\t" << ",#maxFreq: " << maxFreq << endl;
	return ss; 
}

void PatternNeuron::setNeuronParams(vector<string> values) {
	Neuron::setNeuronParams(values);
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#Frequency") {
			it++;
			tmp = values[it];
			frequency = atof(tmp.c_str());
		}
		else if (tmp == "#Amplitude") {
			it++;
			tmp = values[it];
			amplitude = atof(tmp.c_str());
		}
		else if (tmp == "#Phase") {
			it++;
			tmp = values[it];
			phase = atof(tmp.c_str());
		}
	}
}

bool PatternNeuron::checkNeuron(vector<string> values) {
	for (int it = 0; it < values.size(); it++) {
		string tmp = values[it];
		if (tmp == "#Frequency") {
			it++;
			tmp = values[it];
			if ((frequency + 0.001) < atof(tmp.c_str()) || (frequency - 0.001) > atof(tmp.c_str())) {
				cout << "ERROR: frequency differs" << endl;
				cout << "frequency = " << frequency << ", while file says: " << atof(tmp.c_str()) << endl;
			}
		}
		else if (tmp == "#Amplitude") {
			it++;
			tmp = values[it];
			if ((amplitude + 0.001) < atof(tmp.c_str()) || (amplitude - 0.001) > atof(tmp.c_str())) {
				cout << "ERROR: Amplitude differs " << endl;
				cout <<  "amplitude = " << amplitude << ", while file says: " << atof(tmp.c_str()) << endl;
				return false;
			}
		}
	}
	return Neuron::checkNeuron(values);
}