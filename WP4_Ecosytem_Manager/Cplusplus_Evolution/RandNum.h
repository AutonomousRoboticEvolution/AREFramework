#pragma once

class RandNum
{
public:
	RandNum(int seed); // instantiate the class and specify the initial seed. 
	~RandNum();
	float randFloat(float lower, float upper); // creates a random float between two specified values. 
	int randInt(int range, int offset); // creates a random integer (range, offset)
	void setSeed(int seed); // sets the seed of the random number generator
	int m_seed = 0;
	int getSeed();
};

