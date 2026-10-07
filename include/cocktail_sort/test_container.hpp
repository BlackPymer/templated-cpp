#pragma once
#include <cstddef>
#include <vector>

class TestContainer : public std::vector<int> {
    public:
    using base_type = std::vector<int>;
    using value_type = typename base_type::value_type;
    using reference = typename base_type::reference;
    using const_reference = typename base_type::const_reference;
    using iterator = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;
    using size_type = typename base_type::size_type;
    using difference_type = typename base_type::difference_type;

    TestContainer() = default;
    TestContainer(std::initializer_list<value_type> values) : base_type(values) {}
    template <typename InputIt>
    TestContainer(InputIt first, InputIt last) : base_type(first, last) {}
};
