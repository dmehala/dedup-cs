#include "dedup/er.hpp"

namespace er {

void block_purging(Blocks& blocks, const int max, const int min=0)
{
    for (auto it = blocks.cbegin(); it != blocks.cend(); ) {
        const auto cardinality = it->size();
        if (cardinality < min || cardinality > max)
            it = blocks.erase(it);
        else
            ++it;
    }
}

}