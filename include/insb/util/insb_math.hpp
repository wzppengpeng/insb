#ifndef INSB_MATH_HPP_
#define INSB_MATH_HPP_

#include <cmath>
#include <random>

#include "insb/common.hpp"

namespace insb
{

namespace math
{


/**
 * clip a number, let the value in a right range
 */
template<typename T>
inline T insb_clip(T& val, T L, T H) {
    if(val < L) {
        val = L;
    } else if (val > H) {
        val = H;
    }
    return val;
}


template<typename T = unsigned>
inline T insb_random_int(T begin, T end) {
    static std::random_device rd;
    static std::default_random_engine e(rd());
    std::uniform_int_distribution<T> u(begin, end);
    return u(e);
}


/**
 * generate random float or double
 * @return       float random value
 */
template<typename T>
inline F insb_random_real(T begin, T end) {
    static std::default_random_engine er;
    static std::uniform_real_distribution<F> ur(begin, end);
    return ur(er);
}

/**
 * math functions
 */
template<typename T>
void insb_l2_norm(std::vector<T>& v) {
    T sum_val = 0.;
    for(size_t i = 0; i < v.size(); ++i) {
        sum_val += v[i] * v[i];
    }
    for(size_t i = 0; i < v.size(); ++i) {
        v[i] /= sqrt(sum_val);
    }
}

} //math

} //insb


#endif /*INSB_MATH_HPP_*/