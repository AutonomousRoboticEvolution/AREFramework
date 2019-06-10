#pragma once
#include "FixedBaseMorphology.h"

class Component : public FixedBaseMorphology {
    struct SITE {
        float x;
        float y;
        float z;
        float rX;
        float rY;
        float rZ;
        int parentHandle;
        int relativePosHandle;
    };
};



