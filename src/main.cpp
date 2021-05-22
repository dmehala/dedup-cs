#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

#include "dedup/er.hpp"
#include "dedup/helper.hpp"

namespace fs = std::filesystem;

class Movie
{
public:
    Movie(std::vector<std::string>&& entry)
        : id(entry[0])
        , genre(entry[3])
        , directors(entry[4])
        , actors(entry[5])
        , year(std::stoi(entry[1]))
        , length(std::stoi(entry[2]))
    {}

    friend std::ostream& operator<<(std::ostream& os, const Movie& movie);

public:
    std::string id;
    std::string genre;
    std::string directors;
    std::string actors;
    int         year;
    int         length;
};

std::ostream& operator<<(std::ostream& os, const Movie& movie)
{
    os << movie.id << ", " << movie.year << ", " << movie.length << ", " << movie.genre << ", " << movie.directors << ", " << movie.actors;
    return os;
}

er::Blocks make_blocks(const std::vector<Movie>& movies)
{
    // Block building & block refinement
    auto blocking_key = [](const Movie& m) { return m.actors; };
    auto blocks = er::make_standard_blocks(movies, blocking_key);
    er::block_purging(blocks, 2, 2);

    er::Blocks final_blocks;
    for (auto it = blocks.cbegin(); it != blocks.cend(); ++it)
        final_blocks.emplace_back(it->second);

    return final_blocks;
}

inline double step_similarity(const int x, const int y, const int offset) {
    return static_cast<double>(std::abs(x - y) <= offset);
}

inline double linear_similarity(const int x, const int y, const double scale) {
    return static_cast<double>(((2 * std::abs(x - y)) / (x + y)) <= scale);
}

double similarity_function(const Movie& rhs, const Movie& lhs, const std::vector<double>& weights)
{
    // avg = sum(a*weights) / sum(weights)
    // Ex: id, year, length, genre, directors, actors
    //      0,  1.0,      1,     0,       1.0,    1.0
    // 
    // weight: 0, 30, 30, 15, 15, 10
    // 
    // avg = (1*30 + 1*30 + 0*15 + 1*15 + 1*10) / 100 = 0.85
    const auto sz = weights.size();

    std::vector<double> tmp;
    tmp.reserve(weights.size());
    tmp.push_back(step_similarity(rhs.year, lhs.year, 1));
    tmp.push_back(linear_similarity(rhs.length, lhs.length, 0.05));
    tmp.push_back(helper::lcs(rhs.genre, lhs.genre));
    tmp.push_back(helper::lcs(rhs.directors, lhs.directors));
    tmp.push_back(helper::lcs(rhs.actors, lhs.actors));

    double sum = 0.f;
    for (int i = 0; i < weights.size(); ++i) {
        sum += tmp[i] * weights[i];
    }

    return sum / 100;
}

void print_usage(const char* const bin)
{
    std::cout << "Usage: " << bin << " <path-to-tsv>" << std::endl << std::endl
              << "Generate result.tsv which contains a list of duplicated entries." << std::endl << std::endl;
}

int main(int argc, char* argv[])
{
    const char* const bin = argv[0];

    if (argc < 2) {
        print_usage(bin);
        std::cout << "Error: missing input." << std::endl;
        return 0;
    }

    const char* const tsv_file = argv[1];
    if (!fs::exists(tsv_file)) {
        print_usage(bin);
        std::cout << "Error: input " << tsv_file << " does not exists." << std::endl;
        return 0;
    }

    std::ifstream input(tsv_file, std::ios::in);
    if (!input.is_open()) {
        print_usage(bin);
        std::cout << "Error: failed to open " << tsv_file << '.' << std::endl;
        return 0;
    }

    // TODO: Encapsulate logic in csv.hpp
    std::vector<Movie> movies;
    std::string header;
    std::getline(input, header);
    
    // str -> idx
    // std::unordered_map<std::string, int> header_map;
    // auto headers = split(std::move(header), '\t', 6);
    // const int sz_headers = headers.size();
    // for (int i = 0; i < sz_headers; ++i) {
    //     header_map[headers[i]] = i;
    // }
    // How to use: but needs to override operator[]
    // entry['year']

    for (std::string line; std::getline(input, line);)
        movies.emplace_back(helper::split(std::move(line), '\t', 6));

    std::cout << "Movie collection size: " << movies.size() << std::endl;
    er::Blocks blocks = make_blocks(movies);

    const double threshold = 0.85;
    const std::vector<double> weights { 30, 30, 15, 15, 10 };
    
    // Expected: 279229 duplicates
    // Found: 102938 duplicates
    const std::vector<std::pair<int, int>> duplicates = er::iterative_blocking(movies, blocks, similarity_function, weights, threshold);
    std::cout << duplicates.size() << " duplicates found" << std::endl;

    if (!duplicates.empty()) {
        std::ofstream output("result.tsv", std::ios::out);
        if (!output.is_open())
            throw std::runtime_error("Error: failed to write result.tsv");

        for (const auto& p : duplicates)
            output << movies[p.first].id << "\t" << movies[p.second].id << "\n";
    }

    return 0;
}