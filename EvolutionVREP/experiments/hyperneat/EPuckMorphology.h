#ifndef EPOCHMORPHOLOGY_H
#define EPOCHMORPHOLOGY_H

#include "ARE/Morphology.h"

namespace are {

class EPuckMorphology : public Morphology
{
public:
    Morphology::Ptr clone() const override
        {return std::make_shared<EPuckMorphology>(*this);}

    void create() override;
    void createAtPosition(float,float,float) override;
    void update() override {}
    void saveBaseMorphology(int,float) override {}
    void loadBaseMorphology(int,int) override {}

    void setPosition(float,float,float);

};

}

#endif //EPOCHMORPHOLOGY_H
