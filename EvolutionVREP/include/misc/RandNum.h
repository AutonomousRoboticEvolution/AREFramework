#pragma once
#ifndef RANDNUM_H
#define RANDNUM_H

#include <memory>

namespace misc {

class RandNum
{
public:
    typedef std::shared_ptr<RandNum> Ptr;
    typedef std::shared_ptr<const RandNum> ConstPtr;

	RandNum(int seed); // instantiate the class and specify the initial seed. 
	~RandNum();
	float randFloat(float lower, float upper); // creates a random float between two specified values. 
	int randInt(int range, int offset); // creates a random integer (range, offset)
	void setSeed(int seed); // sets the seed of the random number generator
	int m_seed = 0;
	int getSeed();
};

}//misc

#endif //RANDNUM_H
