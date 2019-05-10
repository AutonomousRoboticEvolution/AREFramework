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

class Skeleton : public Component
{
public:
    int skeletonType;
    int skeletonHandle;
    int parentHandle;
    int childHandle;
    vector<Organ> organs;
    void createSkeleton();
private:
    void createBars();
};