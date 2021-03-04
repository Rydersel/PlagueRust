#ifndef _MATH_HPP
#define _MATH_HPP

/* This header is used to import all of the math functions */

/* Includes */
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Matrix.hpp"
#include <cstdint>
#include <vector>
#include <map>
#include <mutex>

bool InRange(float min, float max, float val);

/* A thread-safe std::map */
template <typename TKey, typename TVal>
struct ThreadMap
{
    std::map<TKey, TVal> map;
    std::mutex mutex;
};

/* A thread-safe std::vector */
template <typename Type>
struct ThreadVec
{
    std::vector<Type> vec;
    std::mutex mutex;
};

#endif // _MATH_HPP