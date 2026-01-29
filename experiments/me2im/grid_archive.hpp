#pragma once
#include <iostream>
#include <vector>
#include <tuple>
#include <functional>
#include <map>
#include <cmath>
#include <stdexcept>
#include <ARE/misc/RandNum.h>

namespace are{

/**
 * @brief Grid archive to store solutions from MAP-Elites and similar algorithms.
 */
template<typename T>
class GridArchive {
public:
    using comparator_t = std::function<bool(const T&, const T&)> ;

    GridArchive(){}
    GridArchive(const std::vector<int>& bins, const std::vector<std::tuple<double,double>>& boundaries,comparator_t comparator)
        : _bins(bins), _boundaries(boundaries), _comparator(comparator) {
        _dimensions = bins.size();
    }
    void reset() {
        _solutions.clear();
        _indices.clear();
    }
    bool add_solution(const T& solution, const std::vector<double>& descriptor) {
        if (descriptor.size() != _dimensions) {
            return false; // Descriptor dimension mismatch
        }
        std::vector<int> index(_dimensions);
        for (int i = 0; i < _dimensions; ++i) {
            double min_bound = std::get<0>(_boundaries[i]);
            double max_bound = std::get<1>(_boundaries[i]);
            if (descriptor[i] < min_bound || descriptor[i] > max_bound) {
                return false; // Descriptor out of bounds
            }
            double normalized = (descriptor[i] - min_bound) / (max_bound - min_bound);
            index[i] = std::min(static_cast<int>(normalized * _bins[i]), _bins[i] - 1);
        }

        if (_indices.find(index) == _indices.end()) {
            _solutions.push_back(solution);
            _indices[index] = static_cast<int>(_solutions.size()-1);
            return true;
        }else if(_comparator(solution, _solutions[_indices[index]])) {
            _solutions[_indices[index]] = solution;
            return true;
            
        }
        return false; // Slot already occupied
    }
    const T& random_solution(const misc::RandNum::Ptr &rand_num) const {
        if (_solutions.empty()) {
            throw std::runtime_error("No solutions in archive");
        }
        int index = rand_num->randInt(0, static_cast<int>(_solutions.size()) - 1);
        return _solutions[index];
    }

    const std::vector<T>& get_solutions() const {
        return _solutions;
    }

    const std::map<std::vector<int>,int>& get_indices() const {
        return _indices;
    }

    size_t size() const {
        return _solutions.size();
    }

private:
    int _dimensions; // Number of dimensions
    std::vector<int> _bins; // Number of bins per dimension
    std::vector<std::tuple<double,double>> _boundaries; // Min and max for each dimension
    std::map<std::vector<int>,int> _indices; // Matrix of indices organised as a multi-dimensional grid
    std::vector<T> _solutions; // Vector of solutions
    comparator_t _comparator; // Function to compare solutions quality
};

}//are
