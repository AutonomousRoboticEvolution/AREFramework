#ifndef FIXED_MORPHOLOGY_HPP
#define FIXED_MORPHOLOGY_HPP

#include "ARE/Morphology.h"
#if defined(VREP)
#include "v_repLib.h"
#elif defined (COPPELIASIM)
#include "simLib.h"
#endif

namespace are {

struct subtrates{
    static NEAT::Substrate are_puck;
    static NEAT::Substrate epuck;
};

class FixedMorphology : public Morphology
{
public:
    FixedMorphology(const settings::ParametersMapPtr &param) : Morphology(param){}
    Morphology::Ptr clone() const override
        {return std::make_shared<FixedMorphology>(*this);}

    void create() override;
    void createAtPosition(float,float,float) override;
    std::vector<double> update() override;

    void loadModel();
    void setSubstrate(NEAT::Substrate sub){
        substrate = sub;
    }


    void setPosition(float,float,float);
    std::vector<int> get_jointHandles(){return jointHandles;}

protected:
    void getObjectHandles() override;

private:
    std::vector<int> jointHandles;
    std::vector<int> proxHandles;
    int cameraHandle;
};

}

#endif //FIXED_MORPHOLOGY_HPP
