#pragma once

#include "Component.h"
#include <vector>

enum OrganType{
    BRAINORGAN = 0,
    MOTORORGAN = 1,
    SENSORORGAN = 2,
    JOINTORGAN = 3,
};

class Organ : public Component
{
public:
    int organType;
    int organHandle;
    int forceSensorHandle;
    int parentHandle;
    vector<float> coordinates;
    vector<float> orientations;
    void createOrgan();

private:
    void loadOrganModel();
};



