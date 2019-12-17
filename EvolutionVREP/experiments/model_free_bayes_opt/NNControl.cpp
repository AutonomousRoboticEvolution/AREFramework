#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    nn.Flush();
    nn.Input(sensorValues);
    nn.ActivateUseInternalBias();
    return nn.Output();
}
const Eigen::VectorXd &NNControl::get_weights(){
    Eigen::VectorXd weights(nn.m_connections.size());
    size_t i = 0;
    for(const auto &conn : nn.m_connections){
        weights(i) = conn.m_weight;
        i++;
    }
}
