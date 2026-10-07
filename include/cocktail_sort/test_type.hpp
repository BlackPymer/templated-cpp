#pragma once
#include <ostream>

class TestType {
    public:
    int value;
    int id;

    TestType(int value, int id = 0) : value(value), id(id) {}

    bool operator<(const TestType &other) const { return value < other.value; }
    bool operator==(const TestType &other) const { return value == other.value; }
    bool operator!=(const TestType &other) const { return !(*this == other); }
};

inline std::ostream &operator<<(std::ostream &os, const TestType &item) {
    return os << item.value << ':' << item.id;
}
