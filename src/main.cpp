#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

#include "dedup/csv.hpp"
#include "dedup/er.hpp"
#include "dedup/helper.hpp"

namespace fs = std::filesystem;

class Movie
{
public:
    Movie(csv::Row& entry)
        : id(entry["id"])
        , actors(entry["actors"])
        , year(std::stoi(entry["year"]))
        , length(std::stoi(entry["length"]))
        , genres(helper::split(entry["genre"], ','))
        , directors(helper::split(entry["directors"], ','))
        , actors_set(helper::split(entry["actors"], ','))
    {
        std::sort(genres.begin(), genres.end());
        std::sort(directors.begin(), directors.end());
        std::sort(actors_set.begin(), actors_set.end());
    }

public:
    std::string id;
    std::string actors;
    int         year;
    int         length;
    std::vector<std::string> genres;
    std::vector<std::string> directors;
    std::vector<std::string> actors_set;
};

// @brief Returns a score describing how similar two movies are.
double similarity_function(const Movie& rhs, const Movie& lhs, const std::vector<double>& weights)
{
    // avg = sum(a*weights) / sum(weights)
    double sum = 0.f;
    sum += helper::step_similarity(rhs.year, lhs.year, 1)               * weights[0];
    sum += helper::linear_similarity(rhs.length, lhs.length, 0.05)      * weights[1];
    sum += helper::jaccard_similarity(rhs.genres, lhs.genres)           * weights[2];
    sum += helper::jaccard_similarity(rhs.directors, lhs.directors)     * weights[3];
    sum += helper::jaccard_similarity(rhs.actors_set, lhs.actors_set)   * weights[4];
    
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

    std::vector<Movie> movies;
    csv::Reader reader(input, '\t');
    for (csv::Row& row : reader)
        movies.emplace_back(row);

    std::cout << "Movie collection size: " << movies.size() << std::endl;

    // Block building & block refinement
    auto blocking_key = [](const Movie& m) { return m.actors; };
    er::Blocks blocks = er::make_standard_blocks(movies, blocking_key);
    er::block_purging(blocks, 2, 2);

    const double threshold = 0.75;
    const std::vector<double> weights { 30, 30, 10, 15, 15 };
    
    // Expected duplicates: 279229 duplicates
    const std::vector<std::pair<int, int>> duplicates = er::matching(movies, blocks, similarity_function, weights, threshold);
    std::cout << duplicates.size() << " duplicates found, check result.tsv for more details." << std::endl;

    if (!duplicates.empty()) {
        std::ofstream output("result.tsv", std::ios::out);
        if (!output.is_open())
            throw std::runtime_error("Error: failed to write result.tsv");

        for (const auto& p : duplicates)
            output << movies[p.first].id << "\t" << movies[p.second].id << "\n";
    }

    return 0;
}