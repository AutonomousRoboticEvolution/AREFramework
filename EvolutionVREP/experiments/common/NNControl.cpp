#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    nn.Flush();
    nn.Input(sensorValues);
    nn.ActivateUseInternalBias();
    return nn.Output();
}
