#pragma once

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <utility>
#include <vector>

template <typename Iterator> class StrandSorter {
    public:
    using iterator_type = Iterator;
    using value_type = typename std::iterator_traits<iterator_type>::value_type;

    void Sort(iterator_type begin, iterator_type end);
};

template <typename Iterator>
void StrandSorter<Iterator>::Sort(iterator_type begin, iterator_type end) {
    if (begin == end || std::next(begin) == end)
        return;

    std::vector<value_type> data(begin, end);
    const std::size_t count = data.size();
    std::vector<bool> taken(count, false);
    std::vector<value_type> sorted;
    sorted.reserve(count);

    std::size_t takenCount = 0;
    while (takenCount < count) {
        std::vector<value_type> run;
        std::size_t i = 0;
        while (i < count && taken[i])
            ++i;
        taken[i] = true;
        ++takenCount;
        run.push_back(std::move(data[i]));
        for (++i; i < count; ++i) {
            if (taken[i])
                continue;
            if (!(data[i] < run.back())) {
                taken[i] = true;
                ++takenCount;
                run.push_back(std::move(data[i]));
            }
        }

        std::vector<value_type> merged;
        merged.reserve(sorted.size() + run.size());
        std::size_t sortedIndex = 0;
        std::size_t runIndex = 0;
        while (sortedIndex < sorted.size() && runIndex < run.size()) {
            if (run[runIndex] < sorted[sortedIndex])
                merged.push_back(std::move(run[runIndex++]));
            else
                merged.push_back(std::move(sorted[sortedIndex++]));
        }
        while (sortedIndex < sorted.size())
            merged.push_back(std::move(sorted[sortedIndex++]));
        while (runIndex < run.size())
            merged.push_back(std::move(run[runIndex++]));
        sorted = std::move(merged);
    }

    std::copy(sorted.begin(), sorted.end(), begin);
}
