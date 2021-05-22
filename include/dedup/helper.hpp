#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace helper {

constexpr int DEFAULT_HINT = 1 << 6;

std::vector<std::string> split(std::string&& s, char delimiter, const int hint=DEFAULT_HINT);

template <typename T>
std::string str_vector(const std::vector<T>& v)
{
    std::ostringstream os;

    auto it = v.cbegin();
    os << *it;

    // it = std::next(it);
    while (++it != v.cend()) 
        os << ", " << *it;

    return os.str();
}

std::vector<std::string> generate_qgram(const std::string& in, const int q);

double lcs(const std::string& a, const std::string& b);

}