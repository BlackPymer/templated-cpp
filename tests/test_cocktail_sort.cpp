#include <UnitTest++/UnitTest++.h>

#include "cocktail_sort/cocktail_sort.hpp"
#include "cocktail_sort/test_container.hpp"
#include "cocktail_sort/test_type.hpp"
#include "strand_sort/strand_sort.hpp"

#include <algorithm>
#include <cstddef>
#include <random>
#include <type_traits>
#include <vector>

namespace {

std::vector<int> randomValues(std::size_t count) {
    std::vector<int> values(count);
    for (std::size_t i = 0; i < count; ++i)
        values[i] = static_cast<int>(i);
    std::shuffle(values.begin(), values.end(), std::mt19937(12345));
    return values;
}

template <typename Sorter, typename Container>
void checkSameAsStdSort(Container values) {
    Container expected = values;
    std::sort(expected.begin(), expected.end());
    Sorter sorter;
    sorter.Sort(values.begin(), values.end());
    CHECK(values == expected);
}

} // namespace

SUITE(SharedSortSuite) {
    TEST(TypeAliasesArePublic) {
        static_assert(std::is_same_v<CocktailSorter<int *>::value_type, int>);
        static_assert(std::is_same_v<CocktailSorter<int *>::iterator_type, int *>);
        static_assert(std::is_same_v<CocktailSorter<int *>::difference_type, std::ptrdiff_t>);
        static_assert(std::is_same_v<StrandSorter<int *>::value_type, int>);
        static_assert(std::is_same_v<StrandSorter<std::vector<int>::iterator>::size_type,
                                     std::make_unsigned_t<std::ptrdiff_t>>);
        CHECK(true);
    }

    TEST(SortsRandomVectorLikeStdSort) {
        checkSameAsStdSort<CocktailSorter<std::vector<int>::iterator>>(randomValues(64));
        checkSameAsStdSort<StrandSorter<std::vector<int>::iterator>>(randomValues(64));
    }

    TEST(SortsCustomObjectsArray) {
        TestType items[] = {TestType(5, 0), TestType(1, 1), TestType(3, 2), TestType(2, 3)};
        const TestType expected[] = {TestType(1, 1), TestType(2, 3), TestType(3, 2), TestType(5, 0)};

        CocktailSorter<TestType *> cocktail;
        cocktail.Sort(items, items + 4);
        CHECK(std::equal(items, items + 4, expected));

        TestType strandItems[] = {TestType(5, 0), TestType(1, 1), TestType(3, 2), TestType(2, 3)};
        StrandSorter<TestType *> strand;
        strand.Sort(strandItems, strandItems + 4);
        CHECK(std::equal(strandItems, strandItems + 4, expected));
    }

    TEST(SortsCustomObjectVector) {
        const std::vector<TestType> expected = {TestType(1, 0), TestType(2, 1), TestType(7, 2),
                                                TestType(9, 3)};
        std::vector<TestType> values = {TestType(9, 3), TestType(2, 1), TestType(7, 2),
                                        TestType(1, 0)};

        CocktailSorter<std::vector<TestType>::iterator> cocktail;
        cocktail.Sort(values.begin(), values.end());
        CHECK(values == expected);

        values = {TestType(9, 3), TestType(2, 1), TestType(7, 2), TestType(1, 0)};
        StrandSorter<std::vector<TestType>::iterator> strand;
        strand.Sort(values.begin(), values.end());
        CHECK(values == expected);
    }

    TEST(SortsCustomContainer) {
        TestContainer values = {9, 4, 7, 1, 8};
        CocktailSorter<TestContainer::iterator> cocktail;
        cocktail.Sort(values.begin(), values.end());
        CHECK(values == TestContainer({1, 4, 7, 8, 9}));

        TestContainer other = {9, 4, 7, 1, 8};
        StrandSorter<TestContainer::iterator> strand;
        strand.Sort(other.begin(), other.end());
        CHECK(other == TestContainer({1, 4, 7, 8, 9}));
    }

    TEST(BothAlgorithmsAreStable) {
        const std::vector<TestType> input = {TestType(2, 0), TestType(1, 1), TestType(2, 1),
                                             TestType(1, 0), TestType(2, 2)};
        const std::vector<int> expectedIds = {1, 0, 0, 1, 2};

        std::vector<TestType> cocktailValues = input;
        CocktailSorter<std::vector<TestType>::iterator> cocktail;
        cocktail.Sort(cocktailValues.begin(), cocktailValues.end());
        std::vector<int> cocktailIds;
        for (const TestType &item : cocktailValues)
            cocktailIds.push_back(item.id);
        CHECK(cocktailIds == expectedIds);

        std::vector<TestType> strandValues = input;
        StrandSorter<std::vector<TestType>::iterator> strand;
        strand.Sort(strandValues.begin(), strandValues.end());
        std::vector<int> strandIds;
        for (const TestType &item : strandValues)
            strandIds.push_back(item.id);
        CHECK(strandIds == expectedIds);
    }
}

SUITE(CocktailSortSuite) {
    TEST(SortsArray) {
        int values[] = {64, 34, 25, 12, 22, 11};
        const std::vector<int> expected = {11, 12, 22, 25, 34, 64};
        CocktailSorter<int *> sorter;
        sorter.Sort(values, values + 6);
        CHECK(std::equal(values, values + 6, expected.begin()));
    }

    TEST(SortsReverseOrder) {
        std::vector<int> values = {5, 4, 3, 2, 1};
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2, 3, 4, 5}));
    }

    TEST(KeepsSortedOrder) {
        std::vector<int> values = {1, 2, 3, 4, 5};
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2, 3, 4, 5}));
    }

    TEST(SortsSingleElement) {
        std::vector<int> values = {42};
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({42}));
    }

    TEST(SortsEmptyRange) {
        std::vector<int> values;
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values.empty());
    }

    TEST(SortsTwoElements) {
        std::vector<int> values = {2, 1};
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({1, 2}));
    }

    TEST(SortsDuplicatesAndNegatives) {
        std::vector<int> values = {3, -1, 0, 3, -5, 0, 7, -1};
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<int>({-5, -1, -1, 0, 0, 3, 3, 7}));
    }

    TEST(SortsSubrangeOnly) {
        std::vector<int> values = {9, 9, 3, 1, 9, 9};
        CocktailSorter<std::vector<int>::iterator> sorter;
        sorter.Sort(values.begin() + 2, values.end() - 2);
        CHECK(values == std::vector<int>({9, 9, 1, 3, 9, 9}));
    }

    TEST(SortsDoublyLinkedListStyleSequence) {
        std::vector<char> values = {'d', 'a', 'c', 'b'};
        CocktailSorter<std::vector<char>::iterator> sorter;
        sorter.Sort(values.begin(), values.end());
        CHECK(values == std::vector<char>({'a', 'b', 'c', 'd'}));
    }
}
