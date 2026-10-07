#include <UnitTest++/UnitTest++.h>

#include "strand_sort/strand_sort.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

SUITE(StrandSortSuite) {
    TEST(SortsArray) {
        int values[] = {64, 34, 25, 12, 22, 11};
        const std::vector<int> expected = {11, 12, 22, 25, 34, 64};
        StrandSorter<int *> sorter;
        sorter.Sort(values, values + 6);
        CHECK(std::equal(values, values + 6, expected.begin()));
    }

    TEST(SortsReverseOrder) {
        std::vector<int> values = {5, 4, 3, 2, 1};
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2, 3, 4, 5}));
    }

    TEST(KeepsSortedOrder) {
        std::vector<int> values = {1, 2, 3, 4, 5};
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2, 3, 4, 5}));
    }

    TEST(SortsSingleElement) {
        std::vector<int> values = {42};
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({42}));
    }

    TEST(SortsEmptyRange) {
        std::vector<int> values;
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values.empty());
    }

    TEST(SortsTwoElements) {
        std::vector<int> values = {2, 1};
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2}));
    }

    TEST(SortsDuplicatesAndNegatives) {
        std::vector<int> values = {3, -1, 0, 3, -5, 0, 7, -1};
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({-5, -1, -1, 0, 0, 3, 3, 7}));
    }

    TEST(SortsAlreadyTakenRuns) {
        std::vector<int> values = {1, 2, 3, 10, 4, 5, 6};
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2, 3, 4, 5, 6, 10}));
    }

    TEST(SortsLargeShuffledRange) {
        std::vector<int> values;
        for (int i = 0; i < 100; ++i)
            values.push_back((i * 37) % 101);
        const std::vector<int> expected = [&values] {
            std::vector<int> copy = values;
            std::sort(copy.begin(), copy.end());
            return copy;
        }();
        StrandSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == expected);
    }

    TEST(SortsStringVector) {
        std::vector<std::string> values = {"delta", "alpha", "charlie", "bravo", "alpha"};
        StrandSorter<std::vector<std::string>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values ==
              std::vector<std::string>({"alpha", "alpha", "bravo", "charlie", "delta"}));
    }
}
