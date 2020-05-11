#include "misc/RandNum.h"
#include <iostream>
#include <cmath>

using namespace misc;

RandNum::RandNum(int seed)
{
    gen.seed(seed);
    std::cout << "seed set to " << seed << std::endl;
    m_seed = seed;
}


RandNum::~RandNum()
{
}

float RandNum::randFloat(float lower, float upper) {
    boost::random::uniform_real_distribution<> dist(lower,upper);
    return dist(gen);
}

int RandNum::randInt(int lower, int upper) {
    boost::random::uniform_int_distribution<> dist(lower,upper);
    return dist(gen);
}

void RandNum::setSeed(int seed) {
    gen.seed(seed);
	m_seed = seed;
	std::cout << "Seed set to " << seed << std::endl;
}

int RandNum::getSeed()
{
        return m_seed;
}
