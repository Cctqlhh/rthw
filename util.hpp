
#ifndef __DSTARLITE_UTIL_HPP__
#define __DSTARLITE_UTIL_HPP__

#include <vector>
#include <utility>
#include <algorithm>

template<typename T1, typename T2>
std::pair<T1, T2> operator+(const std::pair<T1, T2> &p1, const std::pair<T1, T2> &p2)
{
  return {p1.first + p2.first, p1.second + p2.second};
}

template<typename T1, typename T2>
std::pair<T1, T2> operator-(const std::pair<T1, T2> &p1, const std::pair<T1, T2> &p2)
{
  return {p1.first - p2.first, p1.second - p2.second};
}

template<typename T1, typename T2> std::pair<T1, T2> operator-(const std::pair<T1, T2> &p)
{
  return {-p.first, -p.second};
}

template<typename T> size_t arg_min(std::vector<T> v)
{
  return std::min_element(v.begin(), v.end()) - v.begin();
}

#endif /* __DSTARLITE_UTIL_HPP__ */