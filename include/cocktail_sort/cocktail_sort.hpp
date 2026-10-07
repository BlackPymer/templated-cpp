#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

template <typename T> class CocktailSorter {
    public:
    // публичные псевдонимы используемых типов (см. ТЗ)
    using iterator_type = T;
    using iterator_category = typename std::iterator_traits<iterator_type>::iterator_category;
    using value_type = typename std::iterator_traits<iterator_type>::value_type;
    using reference = typename std::iterator_traits<iterator_type>::reference;
    using pointer = typename std::iterator_traits<iterator_type>::pointer;
    using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
    using size_type = std::make_unsigned_t<difference_type>;

    void Sort(iterator_type begin, iterator_type end);
};

template <typename T> void CocktailSorter<T>::Sort(iterator_type begin, iterator_type end) {
    if (begin == end || std::next(begin) == end)
        return;
    iterator_type l = begin;
    iterator_type r = std::prev(end);

    while (l < r) {
        iterator_type new_l = r;
        iterator_type new_r = l;
        for (iterator_type i = l; i != r; ++i) {
            if (*std::next(i) < *i) {
                std::swap(*i, *std::next(i));
                new_r = i;
            }
        }
        r = new_r;
        if (l >= r)
            break;
        for (iterator_type i = r; i != l; --i) {
            if (*i < *std::prev(i)) {
                std::swap(*i, *std::prev(i));
                new_l = i;
            }
        }
        l = new_l;
    }
}
