#pragma once

#include <iterator>
#include <utility>

template <typename T> class CocktailSorter {
  public:
    void Sort(T begin, T end);
};

template <typename T> void CocktailSorter<T>::Sort(T begin, T end) {
    if (begin == end || std::next(begin) == end)
        return;
    T l = begin;
    T r = std::prev(end);

    while (l < r) {
        T new_l = r;
        T new_r = l;
        for (T i = l; i != r; ++i) {
            if (*std::next(i) < *i) {
                std::swap(*i, *std::next(i));
                new_r = i;
            }
        }
        r = new_r;
        if (l >= r)
            break;
        for (T i = r; i != l; --i) {
            if (*i < *std::prev(i)) {
                std::swap(*i, *std::prev(i));
                new_l = i;
            }
        }
        l = new_l;
    }
}
