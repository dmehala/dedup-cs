#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace helper {

const char ws[]             = " \n\r\f\v";
constexpr int DEFAULT_HINT  = 1 << 6;

std::vector<std::string> split(std::string s, char delimiter, const int hint=DEFAULT_HINT);

inline void rtrim(std::string& s, const char* const _ws = ws)
{
    s.erase(s.find_last_not_of(_ws) + 1);
}

std::vector<std::string> generate_qgram(const std::string& in, const int q);

double lcs(const std::string& a, const std::string& b);

}