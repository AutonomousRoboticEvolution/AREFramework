#include "ARE/Settings.h"

using namespace are;

settings::ParametersMapPtr settings::defaults::parameters = std::make_shared<settings::ParametersMap>(
            []() -> settings::ParametersMap{
                settings::ParametersMap parameters;
                parameters.emplace("#experimentName",new settings::String("neuroEvolution"));
                parameters.emplace("#expPluginName",new settings::String("/usr/local/lib/libneuroEvolution.so"));
                parameters.emplace("#scenePath",new settings::String("~/evolutionary_robotics_framework/EvolutionVREP/experiments/sim/ARE_arena.ttt"));
                parameters.emplace("#robotPath",new settings::String("~/evolutionary_robotics_framework/EvolutionVREP/experiments/sim/ARE-puck.ttm"));
                parameters.emplace("#repository",new settings::String("~/are_logs"));
                parameters.emplace("#populationSize",new settings::Integer(50));
                parameters.emplace("#maxEvalTime",new settings::Float(60));
                parameters.emplace("#numberOfGeneration",new settings::Float(1000));
                parameters.emplace("#maxEvalTime",new settings::Float(60));
                parameters.emplace("#timeStep",new settings::Float(0.05));
                parameters.emplace("#verbose",new settings::Boolean(0));
                parameters.emplace("#instanceType",new settings::Integer(0));
                parameters.emplace("#seed",new settings::Integer(-1));
                return parameters;
            }());

settings::ParametersMap settings::loadParameters(const std::string& file)
{
    settings::ParametersMap parameters;
    std::ifstream ifs(file);
    if(!ifs || !ifs.good())
    {
        std::cerr << "Unable to open parameters file : " << file << std::endl;
        return settings::ParametersMap();
    }
    std::string line;
    std::list<std::string> values;
    std::vector<std::string> param;
    while(std::getline(ifs,line)){
        param.clear();
        values.clear();
        misc::split_line(line,",",values);

        for(auto v : values)
            param.push_back(v);

        Type::Ptr val = buildType(param[1]);
        val->fromString(param[2]);
        parameters.emplace(param[0],val);
    }
    return parameters;
}


settings::Type::Ptr settings::buildType(const std::string &name)
{
    if(name == "bool")
        return std::make_shared<settings::Boolean>(Boolean());
    else if(name == "int")
        return std::make_shared<settings::Integer>(Integer());
    else if(name == "float")
        return std::make_shared<settings::Float>(Float());
    else if(name == "double")
        return std::make_shared<settings::Double>(Double());
    else if(name == "string")
        return std::make_shared<settings::String>(String());
    else
    {
        std::cerr << "unknown parameter type : " << name << std::endl;
        return nullptr;
    }
}
