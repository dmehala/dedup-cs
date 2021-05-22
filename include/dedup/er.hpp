#pragma once

#include <vector>
#include <type_traits>
#include <set>
#include <map>
#include <tuple>
#include <unordered_map>

#include "helper.hpp"

// er = entity resolution
namespace er {

// A block is just a list of index entities
using Block = std::vector<int>;
using Blocks = std::vector<er::Block>;

template <typename E, typename F>
auto make_standard_blocks(const std::vector<E>& entities, F&& generate_bk)
{
    // Standard blocking (Fellegiet. al., JASS 1969)
    using R = typename std::result_of<F(E)>::type;

    const auto sz = entities.size();
    std::map<R, Block> blocks;

    for (int i = 0; i < sz; ++i) {
        const auto& entity = entities[i];

        Block& block = blocks[generate_bk(entity)];
        block.emplace_back(i);
    }

    return blocks;
}

// template <typename E, typename F>
// auto make_custom_standard_blocks(const std::vector<E>& entities, F&& generate_bk)
// {
//     // Standard blocking (Fellegiet. al., JASS 1969)
//     using R = typename std::result_of<F(E)>::type;

//     const auto sz = entities.size();
//     std::map<R, std::vector<int>> blocks;

//     for (int i = 0; i < sz; ++i) {
//         const auto& entity = entities[i];

//         R key = generate_bk(entity);
//         blocks[key-1].emplace_back(i);
//         blocks[key].emplace_back(i);
//         blocks[key+1].emplace_back(i);
//     }

//     return blocks;
// }

template <typename E>
auto make_qgrams_blocks(const std::vector<E>& entities, const int q)
{
    const auto sz = entities.size();
    std::map<std::string, std::vector<int>> blocks;

    for (int i = 0; i < sz; ++i) {
        const auto& entity = entities[i];
        const std::vector<std::string> blocking_keys = generate_qgram(std::to_string(entity.year), q);

        for (const auto& key : blocking_keys)
            blocks[key].emplace_back(i);
    }

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

// Block purging discards blocks that exceed an upper limit on block
// cardinality.
template <typename B>
void block_purging(B& blocks, const int max, const int min=0)
{
    for (auto it = blocks.cbegin(); it != blocks.cend(); ) {
        const auto cardinality = it->second.size();
        if (cardinality < min || cardinality > max)
            blocks.erase(it++);
        else
            ++it;
    }
}

template <typename E, typename F>
std::vector<std::pair<int, int>> iterative_blocking(const std::vector<E>& entities, const Blocks& blocks, F&& similarity_func, const std::vector<double> weights, const double threshold)
{
    std::vector<std::pair<int, int>> result;

    for (const er::Block& b : blocks) {
        const auto block_size = b.size();
        for (auto i = 0; i < block_size; ++i) {
            for (auto j = i + 1; j < block_size; ++j) {
                if (similarity_func(entities[b[i]], entities[b[j]], weights) >= threshold)
                    result.push_back(std::make_pair(b[i], b[j]));
            }
        }
    }

    return result;
}

} // namespace er