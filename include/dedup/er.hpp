#pragma once

#include <vector>
#include <type_traits>
#include <set>
#include <map>
#include <tuple>
#include <unordered_map>
#include <algorithm>

#include "helper.hpp"

// er = entity resolution
namespace er {

// A block is just a list of index entities
using Block = std::vector<int>;
using Blocks = std::vector<er::Block>;

// Block building algorithm
template <typename E, typename F>
Blocks make_standard_blocks(const std::vector<E>& entities, F&& generate_bk)
{
    // Standard blocking (Fellegiet. al., JASS 1969)
    using R = typename std::result_of<F(E)>::type;

    const auto sz = entities.size();
    std::map<R, Block> map;

    for (int i = 0; i < sz; ++i) {
        const auto& entity = entities[i];
        R key = generate_bk(entity);

        Block& block = map[key];
        block.emplace_back(i);
    }

    // auto a = map[" Shinde"];
    // auto b = map["Stephen Ang"];

    Blocks blocks;
    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        Block block = it->second;
        blocks.push_back(block);
    }

    return blocks;
}

template <typename E>
Blocks make_qgrams_blocks(const std::vector<E>& entities, const int q)
{
    const auto sz = entities.size();
    std::map<std::string, std::vector<int>> map;

    for (int i = 0; i < sz; ++i) {
        const auto& entity = entities[i];
        const std::vector<std::string> blocking_keys = generate_qgram(std::to_string(entity.actors), q);

        for (const auto& key : blocking_keys)
            map[key].emplace_back(i);
    }

    Blocks blocks;
    for (auto it = map.cbegin(); it != map.cend(); ++it)
        blocks.emplace_back(it->second);    

    return blocks;
}

// struct less_than_blocks_cardinality
// {
//     inline bool operator() (const Block* const b1, const Block* const b2) {
//         return b1->size() < b2->size();
//     }
// };

// template <typename E, typename B>
// auto block_filtering(const std::vector<E>& entities, const B& blocks)
// {
//     const auto sz = entities.size();

//     // Build entity index
//     std::map<int, std::vector<const Block*>> entity_index;
//     for (auto it = blocks.cbegin(); it != blocks.cend(); ++it) {
//         const Block& block = it->second;
//         for (const auto& idx : block) {
//             auto& list_of_blocks = entity_index[idx];
//             list_of_blocks.push_back(&block);
//             std::sort(list_of_blocks.begin(), list_of_blocks.end(), less_than_blocks_cardinality());
//         }
//     }

//     B filtered_blocks;

//     return filtered_blocks;
// }

// Block refinement algorithm

// @brief Discards blocks that exceed an upper limit
// @details Block purging discards blocks which cardanality that does not meet upper or lower bounds limits 
inline void block_purging(Blocks& blocks, const int max, const int min=0)
{
    blocks.erase(std::remove_if(blocks.begin(), blocks.end(), [min, max](Block& block) { return block.size() < min || block.size() > max; }), blocks.cend());
}

// Filtering/Matching algorithm
template <typename E, typename F>
std::vector<std::pair<int, int>> matching(const std::vector<E>& entities, const Blocks& blocks, F&& similarity_func, const std::vector<double> weights, const double threshold)
{
    std::set<int> seen;
    std::vector<std::pair<int, int>> result;

    for (const er::Block& b : blocks) {
        const auto block_size = b.size();
        for (auto i = 0; i < block_size; ++i) {

            if (seen.find(b[i]) != seen.cend())
                continue;

            for (auto j = i + 1; j < block_size; ++j) {

                if (seen.find(b[j]) != seen.cend())
                    continue;

                if (similarity_func(entities[b[i]], entities[b[j]], weights) >= threshold) {
                    result.push_back(std::make_pair(b[i], b[j]));
                    seen.insert(b[i]);
                    seen.insert(b[j]);
                    break;
                }
            }
        }
    }

    return result;
}

} // namespace er