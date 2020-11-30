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

double RandNum::randDouble(double lower, double upper) {
    std::uniform_real_distribution<> dist(lower,upper);
    return dist(gen);
}


float RandNum::randFloat(float lower, float upper) {
    std::uniform_real_distribution<> dist(lower,upper);
    return dist(gen);
}

int RandNum::randInt(int lower, int upper) {
    std::uniform_int_distribution<> dist(lower,upper);
    return dist(gen);
}

std::vector<double> RandNum::randVectd(double lower, double upper, int size){
    std::vector<double> res;
    for(int i = 0; i < size; i++)
        res.push_back(randDouble(lower,upper));
    return res;
}

double RandNum::normalDist(double mu, double sigma){
    std::normal_distribution<> nd(mu,sigma);
    return nd(gen);
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
