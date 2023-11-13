#include "ARE/Settings.h"

using namespace are;

settings::ParametersMapPtr settings::defaults::parameters = std::make_shared<settings::ParametersMap>(
            []() -> settings::ParametersMap{
                settings::ParametersMap parameters;
                parameters.emplace("#experimentName",std::make_shared<settings::String>("neuroEvolution"));
                parameters.emplace("#expPluginName",std::make_shared<settings::String>("/usr/local/lib/libneuroEvolution.so"));
                parameters.emplace("#scenePath",std::make_shared<settings::String>("~/evolutionary_robotics_framework/EvolutionVREP/experiments/sim/ARE_arena.ttt"));
                parameters.emplace("#repository",std::make_shared<settings::String>("~/are-logs"));
                parameters.emplace("#populationSize",std::make_shared<settings::Integer>(50));
                parameters.emplace("#maxEvalTime",std::make_shared<settings::Float>(60));
                parameters.emplace("#numberOfGeneration",std::make_shared<settings::Float>(1000));
                parameters.emplace("#maxEvalTime",std::make_shared<settings::Float>(60));
                parameters.emplace("#timeStep",std::make_shared<settings::Float>(0.05));
                parameters.emplace("#verbose",std::make_shared<settings::Boolean>(0));
                parameters.emplace("#instanceType",std::make_shared<settings::Integer>(0));
                parameters.emplace("#seed",std::make_shared<settings::Integer>(-1));
                parameters.emplace("#debugDisplayOnPi",std::make_shared<settings::Boolean>(0));
                return parameters;
            }());

settings::ParametersMapPtr settings::random::parameters(std::make_shared<settings::ParametersMap>());

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
    else if(name == "sequence_int")
        return std::make_shared<settings::Sequence<int>>(Sequence<int>());
    else if(name == "sequence_float")
        return std::make_shared<settings::Sequence<float>>(Sequence<float>());
    else if(name == "sequence_double")
        return std::make_shared<settings::Sequence<double>>(Sequence<double>());
    else if(name == "sequence_string")
        return std::make_shared<settings::Sequence<std::string>>(Sequence<std::string>());
    else
    {
        std::cerr << "[ARE error] unknown parameter type : " << name << std::endl;
        return nullptr;
    }
}

settings::ParametersMap settings::loadParameters(const std::string& file)
{
    settings::ParametersMap parameters;
    std::ifstream ifs(file);
    if(!ifs || !ifs.good())
    {
        std::cerr << "[ARE error] unable to open parameters file : " << file << std::endl;
        return settings::ParametersMap();
    }
    std::string line;
    std::list<std::string> values;
    std::vector<std::string> param;
    while(std::getline(ifs,line)){
	
        misc::split_line(line,",",values);
        if(values.empty())
            continue;
        for(auto v : values)
            param.push_back(v);
        if(param.size() != 3){
            param.clear();
            values.clear();
            continue;
        }

        Type::Ptr val = buildType(param[1]);
        val->fromString(param[2]);
        parameters.emplace(param[0],val);
        param.clear();
        param.shrink_to_fit();
        values.clear();
    }
    return parameters;
}



std::string settings::toString(const std::string &name, const settings::Type::ConstPtr& elt){
    std::stringstream sstr;
    if(elt->name == "bool")
        sstr << name << ",bool," << *(settings::cast<settings::Boolean>(elt)).get() << std::endl;
    else if(elt->name == "int")
        sstr << name << ",int," << *(settings::cast<settings::Integer>(elt)).get() << std::endl;
    else if(elt->name == "float")
        sstr << name << ",float," << *(settings::cast<settings::Float>(elt)).get() << std::endl;
    else if(elt->name == "double")
        sstr << name << ",double," << *(settings::cast<settings::Double>(elt)).get() << std::endl;
    else if(elt->name == "string")
        sstr << name << ",string," << *(settings::cast<settings::String>(elt)).get() << std::endl;
    else if(elt->name == "sequence_int")
        sstr << name << ",sequence_int," << *(settings::cast<settings::Sequence<int>>(elt)).get() << std::endl;
    else if(elt->name == "sequence_int")
        sstr << name << ",sequence_float," << *(settings::cast<settings::Sequence<float>>(elt)).get() << std::endl;
    else if(elt->name == "sequence_double")
        sstr << name << ",sequence_double," << *(settings::cast<settings::Sequence<double>>(elt)).get() << std::endl;
    else if(elt->name == "sequence_string")
        sstr << name << ",sequence_string," << *(settings::cast<settings::Sequence<std::string>>(elt)).get() << std::endl;
    return sstr.str();
}

void settings::saveParameters(const std::string& file,const ParametersMapPtr &params){
    //Write parameters in the log folder.
    std::ofstream ofs(file);

    for(const auto &elt : *defaults::parameters)
        if(params->find(elt.first) == params->end())
            ofs << toString(elt.first,elt.second);

    for(const auto &elt : *params)
        if(random::parameters->find(elt.first) == random::parameters->end())
            ofs << toString(elt.first,elt.second);

    for(const auto &elt : *random::parameters)
        ofs << toString(elt.first,elt.second);

    ofs.close();
}

std::string settings::toString(const ParametersMap &params){
    std::stringstream sstr;
    for(const auto &elt : params)
        sstr << toString(elt.first,elt.second);
    return sstr.str();
}

settings::ParametersMap settings::fromString(const std::string &str_params){
    settings::ParametersMap parameters;

    std::istringstream istr(str_params.c_str());

    std::string line;
    std::list<std::string> values;
    std::vector<std::string> param;
    while(std::getline(istr,line)){
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
