#include "dedup/helper.hpp"

namespace helper {

std::vector<std::string> split(std::string s, char delimiter, const int hint)
{
    std::vector<std::string> result;
    result.reserve(hint);

    if (!s.empty()) {
        size_t start = 0;
        size_t end = s.find_first_of(delimiter);
        
        while (end <= std::string::npos)
        {
            result.emplace_back(s.substr(start, end - start));

            if (end == std::string::npos)
                break;

            start = end+1;
            end = s.find_first_of(delimiter, start);
        }
    }

    return result;
}

std::vector<std::string> generate_qgram(const std::string& in, const int q) {
    std::vector<std::string> words;

    auto start = in.begin();
    for (auto end = std::next(start, q); end != in.cend(); ++end, ++start)
        words.emplace_back(start, end);
    words.emplace_back(start, in.cend());

    return words;
}

double lcs(const std::string& a, const std::string& b)
{
    // if (a.size() != b.size())
    //     throw std::runtime_error("Both string must have the same length.");
    const int m = a.size(); // +1
    const int n = b.size(); // +1

    int accumulator[m + 1][n + 1];
    int i, j;
  
    for (i = 0; i <= m; ++i) {
        for (j = 0; j <= n; ++j) {
            if (i == 0 || j == 0)
                accumulator[i][j] = 0;
  
            else if (a[i - 1] == b[j - 1])
                accumulator[i][j] = accumulator[i - 1][j - 1] + 1;
  
            else
                accumulator[i][j] = std::max(accumulator[i - 1][j], accumulator[i][j - 1]);
        }
    }
  
    return accumulator[m][n];
}

} // namespace helper