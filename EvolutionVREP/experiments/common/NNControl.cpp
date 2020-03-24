#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    bool useInternalBias = settings::getParameter<settings::Boolean>(parameters,"#UseInternalBias").value;
    double noiselvl = settings::getParameter<settings::Double>(parameters,"#noiseLevel").value;
    boost::mt19937 rng(randomNum->getSeed());
    std::vector<double> inputs = sensorValues;
    if(noiselvl > 0.0){
        for(double &sv : inputs){
            boost::normal_distribution<> normal(sv,noiselvl);
            sv = normal(rng);
        }
    }

    nn.Flush();

//    std::cout << "sensor values : ";
//    for(double sv : sensorValues)
//        std::cout << sv << "; ";
//    std::cout << std::endl;

    nn.Input(inputs);

    //First activation to activate the hidden layer
    if(useInternalBias)
        nn.ActivateUseInternalBias();
    else nn.Activate();

    //Have to activate a second time to activate the outputs
    if(useInternalBias)
        nn.ActivateUseInternalBias();
    else nn.Activate();

    std::vector<double> output = nn.Output();
    if(noiselvl > 0.0){
        for(double &o : output){
            boost::normal_distribution<> normal(o,noiselvl);
            o = normal(rng);
        }
    }

    return output;
}

