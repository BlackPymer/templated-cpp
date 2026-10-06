#include "cocktail_sort/cocktail_sort.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

int main() {
    const std::vector<int> expected = {11, 12, 22, 25, 34, 64};

    int arr[] = {64, 34, 25, 12, 22, 11};
    const std::size_t size = sizeof(arr) / sizeof(arr[0]);
    CocktailSorter<int *> array_sorter;
    array_sorter.Sort(arr, arr + size);

    std::cout << "array:";
    for (std::size_t i = 0; i < size; ++i) {
        std::cout << ' ' << arr[i];
    }
    std::cout << '\n';

    std::vector<int> vec(64, 0);
    std::iota(vec.begin(), vec.end(), 0);
    std::shuffle(vec.begin(), vec.end(), std::mt19937(42));
    std::vector<int> sorted = vec;
    std::sort(sorted.begin(), sorted.end());

    CocktailSorter<std::vector<int>::iterator> vector_sorter;
    vector_sorter.Sort(vec.begin(), vec.end());

    std::vector<int> reversed(expected.rbegin(), expected.rend());
    vector_sorter.Sort(reversed.begin(), reversed.end());

    std::vector<int> single = {42};
    vector_sorter.Sort(single.begin(), single.end());

    std::vector<int> empty;
    vector_sorter.Sort(empty.begin(), empty.end());

    const bool array_ok = std::equal(arr, arr + size, expected.begin());
    const bool shuffled_ok = vec == sorted;
    const bool reversed_ok = reversed == expected;
    const bool single_ok = single == std::vector<int>{42};
    const bool empty_ok = empty.empty();

    std::cout << "shuffled:";
    for (int value : vec) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';
    std::cout << "reversed:";
    for (int value : reversed) {
        std::cout << ' ' << value;
    }
    std::cout << '\n';

    const bool ok = array_ok && shuffled_ok && reversed_ok && single_ok && empty_ok;
    std::cout << (ok ? "all tests passed" : "tests failed") << '\n';
    return ok ? 0 : 1;
}
