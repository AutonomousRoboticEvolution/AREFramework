#pragma once

#include "Component.h"
#include "Organ.h"
#include <vector>

enum SkeletonType{
    BARS = 0,
    CUBES = 1,
    LSYSTEM = 2,
    CPPN = 3,
};
/**
 * @file Skeleton.h
 * @brief Class declaration of the organs
 */
class Skeleton : public Component
{
public:
    // Defines which organ type to use
    int skeletonType;
    // Stores the skeleton handle
    int skeletonHandle;
    // Stores the parent handle. Might be useful later.
    int parentHandle;
    // Stores the parent handle. Might be useful later.
    int childHandle;
    // Stores all the organs. Useful for direct bars
    vector<Organ> organs;
    /**
     * @brief This method decides which method to use to create skeleton
     */
    void createSkeleton();
private:
    /**
     * @brief This method creates skeleton with Direct Bars technique
     */
    void createBars();
    /**
     * @brief This method creates a cube with Direct Encoding technique
     */
    void createCube();
};