#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <tuple>
#include <unordered_map>
#include <type_traits>

constexpr int DEFAULT_HINT = 1 << 6;

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

std::vector<std::string> split(std::string&& s, char delimiter, const int hint=DEFAULT_HINT)
{
    std::vector<std::string> result;
    result.reserve(hint);

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

    return result;
}

// er = entity resolution
namespace er {

template <typename E, typename F>
auto make_standard_blocks(const std::vector<E>& entities, F&& generate_bk)
{
    // Standard blocking (Fellegiet. al., JASS 1969)
    using R = typename std::result_of<F(E)>::type;

    const auto sz = entities.size();
    std::map<R, std::vector<int>> blocks;

    for (int i = 0; i < sz; ++i) {
        const auto& entity = entities[i];

        auto& block = blocks[generate_bk(entity)];
        block.emplace_back(i);
    }

    return blocks;
}

} // namespace er


std::set<int, int> make_pairs(const std::vector<Movie>& movies)
{
    // const auto blocks = er::make_standard_blocks(movies);
    auto blocking_key = [](const Movie& m) { return m.actors; };
    const auto blocks = er::make_standard_blocks(movies, blocking_key);

    // Block processing / Block refinement
    // We know the collection of movies contains only one single duplicate -> max size of each block is 2

    // for (const auto& m : movies) {
    //     auto it = map.find(m.actors);
    //     if (it == map.cend())
    //         map.emplace("");
    //     else
    //         map.
    // }
    std::set<int, int> result;
    return result;
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
        movies.emplace_back(split(std::move(line), '\t', 6));

    // std::cout << content[1] << std::endl;

    auto pairs = make_pairs(movies);
    // auto pairs = make_pairs(movies, [](const Movie& m) { return m.actors; });
    return 0;
}