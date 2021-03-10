#include "NNControl.h"

using namespace are;

std::vector<double> NNControl::update(const std::vector<double> &sensorValues)
{
    double maxVelocity = settings::getParameter<settings::Double>(parameters,"#maxVelocity").value;
    bool useInternalBias = settings::getParameter<settings::Boolean>(parameters,"#UseInternalBias").value;
    double noiselvl = settings::getParameter<settings::Double>(parameters,"#noiseLevel").value;
    boost::mt19937 rng(randomNum->getSeed());
    std::vector<double> inputs = sensorValues;
    nn.Input(inputs);
//    nn.Flush();

//    std::cout << "sensor values : ";
//    for(double sv : sensorValues)
//        std::cout << sv << "; ";
//    std::cout << std::endl;

    nn.Input(inputs);

    //First activation to activate the hidden layer
    if(useInternalBias)
        nn.ActivateUseInternalBias();
    else nn.Activate();
//    //Have to activate a second time to activate the outputs
//    if(useInternalBias)
//        nn.ActivateUseInternalBias();
//    else nn.Activate();
    
    std::vector<double> output = nn.Output();
    if(noiselvl > 0.0){
        for(double &o : output){
            boost::normal_distribution<> normal(o,noiselvl);
            o = normal(rng);
        }
    }

    for(double &o : output)
        o = o*maxVelocity;

    return output;
}

