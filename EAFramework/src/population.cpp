#include "ARE/population.hpp"


using namespace are;

const Individual::Ptr& Population::at(size_t idx) const
{
    for(size_t i = 0; i < _indexes.size(); i++)
        if(idx == _indexes[i])
            return _individuals.at(i);
    throw std::invalid_argument("Population::at - index not found");
}

Individual::Ptr& Population::operator[](size_t idx)
{
    for(size_t i = 0; i < _indexes.size(); i++)
        if(idx == _indexes[i])
            return _individuals[i];
    throw std::invalid_argument("Population::[] - index not found");

}

const Individual::Ptr& Population::operator[](size_t idx) const
{
    for(size_t i = 0; i < _indexes.size(); i++)
        if(idx == _indexes[i])
            return _individuals.at(i);
    throw std::invalid_argument("const Population::[] - index not found");

}

void Population::erase(size_t idx){
    size_t i = 0;
    for(; i < _indexes.size(); i++)
        if(idx == _indexes[i])
            break;
    if(i == _indexes.size())
        throw std::invalid_argument("Population::erase - index not found");

    _individuals[i].reset();
    _individuals.erase(_individuals.begin() + i);
    _individuals.shrink_to_fit();
    _indexes.erase(_indexes.begin() + i);
    _indexes.shrink_to_fit();
}

void Population::push_back(const Individual::Ptr& ind){
    _individuals.push_back(ind);
    if(_indexes.empty())
        _indexes.push_back(0);
    else _indexes.push_back(_indexes.back()+1);
}

bool Population::check_doublons(){
    for(int i = 0; i < _indexes.size(); i++){
        for(int j = 0; j < _indexes.size(); j++){
            if(i != j && _indexes[i] == _indexes[j])
                return true;
        }
    }
    return false;
}

void Population::resize(size_t s){
    _individuals.resize(s);
    _indexes.resize(s);
    int counter = 0;
    for(int &i: _indexes)
        i = counter++;
}
