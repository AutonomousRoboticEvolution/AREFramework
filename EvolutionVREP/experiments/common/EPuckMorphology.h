#ifndef EPOCHMORPHOLOGY_H
#define EPOCHMORPHOLOGY_H

#include "ARE/Morphology.h"

namespace are {

class EPuckMorphology : public Morphology
{
public:
    EPuckMorphology(const settings::ParametersMapPtr &param) : Morphology(param){}
    Morphology::Ptr clone() const override
        {return std::make_shared<EPuckMorphology>(*this);}

    void create() override;
    void createAtPosition(float,float,float) override;
    std::vector<double> update() override;

    void loadModel();
    void initSubstrate();


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

#endif //EPOCHMORPHOLOGY_H
