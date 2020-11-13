#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    bool useInternalBias = settings::getParameter<settings::Boolean>(parameters,"#UseInternalBias").value;

    nn.Flush();
    nn.Input(sensorValues);

    //First activation to activate the hidden layer
    if(useInternalBias)
        nn.ActivateUseInternalBias();
    else nn.Activate();

    //Have to activate a second time to activate the outputs
    if(useInternalBias)
        nn.ActivateUseInternalBias();
    else nn.Activate();

    return nn.Output();
}

