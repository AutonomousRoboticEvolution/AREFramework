#pragma once

#include "Component.h"
#include <vector>
using namespace std;

class Organ : public Component
{
    public:
        int organType;
	    vector<float> coordinates;
	    vector<float> orientations;    

    void createOrgan(int type, vector<float> coordinates, vector<float> orientations, int* organHandle, int* sensorHandle);
};



