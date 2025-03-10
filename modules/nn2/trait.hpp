//| This file is a part of the nn2 module originally made for the sferes2 framework.
//| Adapted and modified to be used within the ARE framework by Léni Le Goff.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//|
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.




#ifndef _NN_TRAIT_HPP_
#define _NN_TRAIT_HPP_

// for std::pair
#include <map>
#include <valarray>
#include <Eigen/Core>

#ifdef EIGEN3_ENABLED
#include <Eigen/Core>
#endif

#include "params.hpp"
#include "evo_float.hpp"

namespace nn2 {
template<typename T>
struct trait {
    static T zero() {
        return T(0.0f);
    }
    // a 0 initializer for vectors
    static T zero(size_t k) {
        return zero();
    }
    typedef std::valarray<T> vector_t;
    static size_t size(const T& t) {
        return t.size();
    }
    static typename T::type_t single_value(const T& t) {
        assert(t.size() == 1);
        return t.data(0);
    }
    static T opposite(const T& t){
        t.data(0) = -t.data(0);
        return t;
    }
};

template<int Size, typename Params>
struct trait<EvoFloat<Size, Params>>{
    typedef EvoFloat<Size, Params> evo_float_t;
    static std::vector<float> zero() {
        return std::vector<float>(Size,0.0f);
    }
    // a 0 initializer for vectors
    static std::vector<float> zero(size_t k) {
        return zero();
    }
    typedef std::valarray<float> vector_t;
    static size_t size(const evo_float_t&) {
        return Size;
    }
    static std::vector<float> single_value(const evo_float_t& t) {
        return t.data();
    }
    static evo_float_t opposite(const evo_float_t& t) {
        evo_float_t oppo;
        for(int i = 0; i < Size; i++){
            oppo.data(i,-t.data(i));
        }
        return oppo;
    }

};

template<typename Params>
struct trait<nn2::EvoFloat<1, Params>>{
    typedef nn2::EvoFloat<1, Params> evo_float_t;
    static evo_float_t zero() {
        evo_float_t ef;
        ef.data(0,0);
        return ef;
    }
    // a 0 initializer for vectors
    static evo_float_t zero(size_t k) {
        return zero();
    }
    static size_t size(const evo_float_t&) {
        return 1;
    }
    static float single_value(const evo_float_t& t) {
        return t.data(0);
    }
    static evo_float_t opposite(const evo_float_t& t) {
        evo_float_t oppo;
        oppo.data(0,-t.data(0));
        return oppo;
    }
};


template<>
struct trait<params::Dummy> {
    typedef std::valarray<float> vector_t;
    static float zero() {
        return 0.0f;
    }
    static float zero(size_t k) {
        return zero();
    }
    static float single_value(const params::Dummy& t) {
        return 0.0f;
    }
    static size_t size(const params::Dummy&) {
        return 0;
    }
    static params::Dummy opposite(const params::Dummy& p) {
        return p;
    }
};


// go with eigen with double
template<>
struct trait<double> {
    typedef Eigen::VectorXd vector_t;
    static float zero() {
        return 0.0f;
    }
    static vector_t zero(size_t k) {
        return Eigen::VectorXd::Zero(k);
    }
    static float single_value(const double& t) {
        return t;
    }
    static size_t size(const double& t) {
        return 1;
    }
    static double opposite(const double& t) {
        return -t;
    }
};


// go with eigen with float
template<>
struct trait<float> {
    typedef Eigen::VectorXf vector_t;
    static float zero() {
        return 0.0f;
    }
    static vector_t zero(size_t k) {
        return Eigen::VectorXf::Zero(k);
    }
    static float single_value(const float& t) {
        return t;
    }
    static size_t size(const float& t) {
        return 1;
    }
    static float opposite(const float& t) {
        return -t;
    }
};

template<>
struct trait<std::pair<float, float> > {
    typedef std::valarray<std::pair<float, float> > vector_t;
    static std::pair<float, float> zero() {
        return std::make_pair(0.0f, 0.0f);
    }
    static std::pair<float, float> zero(size_t k) {
        return zero();
    }
    static float single_value(const std::pair<float, float>& t) {
        return t.first;
    }
    static size_t size(const std::pair<float, float>& t) {
        return 2;
    }
    static std::pair<float, float> opposite(const std::pair<float, float>& t) {
        std::pair<float, float> oppo = std::make_pair(-t.first,-t.second);
        return oppo;
    }
};

// useful but wrong place (?)
template<typename _CharT, typename _Traits>
static std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& ofs, const std::pair<float, float>& p) {
    return ofs<<p.first<<" "<<p.second;
}

template<typename _CharT, typename _Traits>
static std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& ofs, const std::vector<float>& p) {
    for (size_t i = 0; i < p.size(); ++i)
        ofs<<p[i]<<" ";
    return ofs;
}


template<typename T1, typename T2>
static std::istream& operator>>(std::istream& ifs, std::pair<T1, T2>& p) {
    T1 t1;
    T2 t2;
    ifs >> t1;
    ifs >> t2;
    return std::make_pair(t1, t2);
}

}


#endif
