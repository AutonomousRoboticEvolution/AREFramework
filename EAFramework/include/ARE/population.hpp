#ifndef POPULATION_HPP
#define POPULATION_HPP

#include "ARE/Individual.h"

namespace are{


class Population{
public:
    const Individual::Ptr& at(size_t idx) const;
    Individual::Ptr& operator[](size_t idx);
    const Individual::Ptr& operator[](size_t idx) const;
    void erase(size_t idx);
    void push_back(const Individual::Ptr& ind);
    size_t size() const {return _individuals.size();}
    bool empty() const {return _individuals.empty();}
    void clear(){_individuals.clear();
                    _indexes.clear();}
    void resize(size_t s);

    auto begin(){return _individuals.begin();}
    auto end(){return _individuals.end();}
    auto begin() const {return _individuals.begin();}
    auto end() const {return _individuals.end();}
    auto cbegin() const {return _individuals.cbegin();}
    auto cend() const {return _individuals.cend();}

    int get_index(int i) const {return _indexes[i];}
    bool check_doublons();

private:
    std::vector<Individual::Ptr> _individuals;
    std::vector<int> _indexes;
};

}

#endif //POPULATION_HPP
