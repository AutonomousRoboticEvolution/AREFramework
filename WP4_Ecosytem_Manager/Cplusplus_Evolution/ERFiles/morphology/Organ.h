#pragma once

#include "Component.h"
#include <vector>

/**
 * @file Organ.h
 * @brief Class declaration of the organs
 */
enum OrganType{
    BRAINORGAN = 0,
    MOTORORGAN = 1,
    SENSORORGAN = 2,
    JOINTORGAN = 3,
};
class Organ : public Component
{
public:
    // Defines which organ type to use
    int organType;
    // Stores the organ handle
    int organHandle;
    // Stores the force sensor handle
    int forceSensorHandle;
    // Stores the handle of the parent. This might be useful later for encodings such as L-system
    int parentHandle;
    // Stores the coordinates of the organ
    vector<float> coordinates;
    // Stores the orientation of the organ
    vector<float> orientations;

    /**
     * @brief This method creates organ and force sensor.
     */
    void createOrgan();

private:
    /**
     * @brief This method load organ model from .ttm file.
     */
    void loadOrganModel();
};



