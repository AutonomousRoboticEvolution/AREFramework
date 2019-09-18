#include "RandNum.h"
#include <iostream>
#include <cmath>
using namespace std;

RandNum::RandNum(int seed)
{
	srand(seed);
	std::cout << "seed set to " << seed << std::endl;
}


RandNum::~RandNum()
{
}

float RandNum::randFloat(float lower, float upper) {
	float value = lower + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upper - lower)));
	value = roundf(value * 1000000) / 1000000;
//	cout << "randFloat = " << value << endl;
	return value;
}

int RandNum::randInt(int range, int offset) {
	int value = 0;
	if (range != 0) {
		int value = rand() % range + offset;
		return value;
	}
	else {
		cout << "ERROR: random range was 0, check your code" << endl;
		return 0;
	}
//	cout << "randInt = " << value << endl;
	return value;
}

void RandNum::setSeed(int seed) {
	srand(seed);
	m_seed = seed;
	std::cout << "seed set to " << seed << std::endl;
}

int RandNum::getSeed()
{
	return m_seed;
}
