#pragma once

#include <unordered_map>
#include <fstream>

namespace csv {

class Reader final
{
public:
    Reader(std::ifstream& csv_file, const char delimiter=',');

private:
    std::unordered_map<std::string, int> header;
};

}