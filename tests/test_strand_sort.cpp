#include <UnitTest++/UnitTest++.h>

#include "cocktail_sort/test_container.hpp"
#include "cocktail_sort/test_type.hpp"
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
        CHECK(values == std::vector<std::string>({"alpha", "alpha", "bravo", "charlie", "delta"}));
    }
}

SUITE(StrandSorterEdgeCases) {
    TEST(EmptySingleAndMultiRunInputs) {
        const std::vector<int> sortedMulti = {1, 2, 3, 4, 5};

        std::vector<int> vectorEmpty;
        StrandSorter<std::vector<int>::iterator> vectorSorter;
        vectorSorter.Sort(vectorEmpty.begin(), vectorEmpty.end());
        CHECK(vectorEmpty.empty());
        std::vector<int> vectorSingle = {42};
        vectorSorter.Sort(vectorSingle.begin(), vectorSingle.end());
        CHECK(vectorSingle == std::vector<int>({42}));
        std::vector<int> vectorRuns = {5, 1, 4, 2, 3};
        vectorSorter.Sort(vectorRuns.begin(), vectorRuns.end());
        CHECK(vectorRuns == sortedMulti);

        int rawEmpty[1] = {0};
        StrandSorter<int *> rawSorter;
        rawSorter.Sort(rawEmpty, rawEmpty);
        rawSorter.Sort(rawEmpty, rawEmpty + 1);
        int raw[] = {5, 1, 4, 2, 3};
        rawSorter.Sort(raw, raw + 5);
        CHECK(std::equal(raw, raw + 5, sortedMulti.begin()));

        TestContainer containerEmpty;
        StrandSorter<TestContainer::iterator> containerSorter;
        containerSorter.Sort(containerEmpty.begin(), containerEmpty.end());
        CHECK(containerEmpty.empty());
        TestContainer containerSingle = {7};
        containerSorter.Sort(containerSingle.begin(), containerSingle.end());
        CHECK(containerSingle == TestContainer({7}));
        TestContainer containerRuns = {5, 1, 4, 2, 3};
        containerSorter.Sort(containerRuns.begin(), containerRuns.end());
        CHECK(containerRuns == TestContainer({1, 2, 3, 4, 5}));

        std::vector<TestType> typeEmpty;
        StrandSorter<std::vector<TestType>::iterator> typeSorter;
        typeSorter.Sort(typeEmpty.begin(), typeEmpty.end());
        CHECK(typeEmpty.empty());
        std::vector<TestType> typeSingle = {TestType(42)};
        typeSorter.Sort(typeSingle.begin(), typeSingle.end());
        CHECK(typeSingle == std::vector<TestType>({TestType(42)}));
        std::vector<TestType> typeRuns = {TestType(5), TestType(1), TestType(4), TestType(2),
                                          TestType(3)};
        typeSorter.Sort(typeRuns.begin(), typeRuns.end());
        CHECK(typeRuns == std::vector<TestType>(
                              {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5)}));

        TestType rawTypeEmpty[1] = {TestType(0)};
        StrandSorter<TestType *> rawTypeSorter;
        rawTypeSorter.Sort(rawTypeEmpty, rawTypeEmpty);
        rawTypeSorter.Sort(rawTypeEmpty, rawTypeEmpty + 1);
        TestType rawType[] = {TestType(5), TestType(1), TestType(4), TestType(2), TestType(3)};
        rawTypeSorter.Sort(rawType, rawType + 5);
        CHECK(rawType[0] == TestType(1) && rawType[1] == TestType(2) && rawType[2] == TestType(3) &&
              rawType[3] == TestType(4) && rawType[4] == TestType(5));

        std::vector<std::string> stringEmpty;
        StrandSorter<std::vector<std::string>::iterator> stringSorter;
        stringSorter.Sort(stringEmpty.begin(), stringEmpty.end());
        CHECK(stringEmpty.empty());
        std::vector<std::string> stringSingle = {"only"};
        stringSorter.Sort(stringSingle.begin(), stringSingle.end());
        CHECK(stringSingle == std::vector<std::string>({"only"}));
        std::vector<std::string> stringRuns = {"delta", "alpha", "charlie", "bravo", "echo"};
        stringSorter.Sort(stringRuns.begin(), stringRuns.end());
        CHECK(stringRuns ==
              std::vector<std::string>({"alpha", "bravo", "charlie", "delta", "echo"}));

        std::vector<int> skipped = {5, 1, 6};
        vectorSorter.Sort(skipped.begin(), skipped.end());
        CHECK(skipped == std::vector<int>({1, 5, 6}));
        int rawSkipped[] = {5, 1, 6};
        rawSorter.Sort(rawSkipped, rawSkipped + 3);
        CHECK(std::equal(rawSkipped, rawSkipped + 3, std::vector<int>({1, 5, 6}).begin()));
        TestContainer containerSkipped = {5, 1, 6};
        containerSorter.Sort(containerSkipped.begin(), containerSkipped.end());
        CHECK(containerSkipped == TestContainer({1, 5, 6}));
        std::vector<TestType> typeSkipped = {TestType(5), TestType(1), TestType(6)};
        typeSorter.Sort(typeSkipped.begin(), typeSkipped.end());
        CHECK(typeSkipped == std::vector<TestType>({TestType(1), TestType(5), TestType(6)}));
        TestType rawTypeSkipped[] = {TestType(5), TestType(1), TestType(6)};
        rawTypeSorter.Sort(rawTypeSkipped, rawTypeSkipped + 3);
        CHECK(rawTypeSkipped[0] == TestType(1) && rawTypeSkipped[1] == TestType(5) &&
              rawTypeSkipped[2] == TestType(6));
        std::vector<std::string> stringSkipped = {"delta", "alpha", "zulu"};
        stringSorter.Sort(stringSkipped.begin(), stringSkipped.end());
        CHECK(stringSkipped == std::vector<std::string>({"alpha", "delta", "zulu"}));
    }
}
