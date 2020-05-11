#include "ARE/Settings.h"

using namespace are;



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
