#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

namespace helper {

const char ws[]             = " \n\r\f\v";
constexpr int DEFAULT_HINT  = 1 << 6;

std::vector<std::string> split(std::string s, char delimiter, const int hint=DEFAULT_HINT);

std::vector<std::string> generate_qgram(const std::string& in, const int q);

// @brief lcs stands for Longest Common Subsequence
// lcs does not care about the order, that's why I decided to use it
double lcs(const std::string& a, const std::string& b);

inline double step_similarity(const int x, const int y, const int offset) {
    return static_cast<double>(std::abs(x - y) <= offset);
}

inline double linear_similarity(const int x, const int y, const double scale) {
    return static_cast<double>(((2 * std::abs(x - y)) / (x + y)) <= scale);
}

// s1 & s2 MUST be sorted before
template <typename T>
double jaccard_similarity(const std::vector<T>& s1, const std::vector<T>& s2) {
    if (s1.empty() || s2.empty())
        return 0.f;

    std::vector<T> intersection;
    std::set_intersection(s1.cbegin(), s1.cend(), s2.cbegin(), s2.end(), std::back_inserter(intersection));

    if (intersection.empty())
        return 0.f;
    
    std::vector<T> _union;
    std::set_union(s1.cbegin(), s1.cend(), s2.cbegin(), s2.cend(), std::back_inserter(_union));

    return static_cast<double>(intersection.size()) / _union.size();
}

inline void rtrim(std::string& s, const char* const _ws = ws)
{
    s.erase(s.find_last_not_of(_ws) + 1);
}

} // namespace helper