#pragma once
#include "exceptions.hpp"
#include "graph_vertex.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <vector>

template <typename T> class OrientedGraph {
    public:
    using value_type = T;
    using const_reference = const value_type &;
    using vertex_type = GraphVertex<value_type>;
    using edge_type = typename vertex_type::edge_type;
    using edges_type = typename vertex_type::edges_type;
    using container_type = std::vector<vertex_type>;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;

    static constexpr difference_type unknownIndex = -1;

    class const_iterator {
        public:
        using value_type = T;
        using difference_type = typename container_type::difference_type;
        using reference = const value_type &;
        using pointer = const value_type *;
        using iterator_category = std::forward_iterator_tag;

        const_iterator() = default;
        explicit const_iterator(typename container_type::const_iterator it) : it(it) {}

        reference operator*() const { return it->getValue(); }
        pointer operator->() const { return &it->getValue(); }
        const_iterator &operator++() {
            ++it;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++it;
            return tmp;
        }
        bool operator==(const const_iterator &other) const { return it == other.it; }
        bool operator!=(const const_iterator &other) const { return !(*this == other); }

        private:
        typename container_type::const_iterator it;
    };

    using iterator = const_iterator;

    OrientedGraph() = default;
    OrientedGraph(const OrientedGraph &) = default;
    OrientedGraph(OrientedGraph &&) noexcept = default;
    OrientedGraph &operator=(const OrientedGraph &) = default;
    OrientedGraph &operator=(OrientedGraph &&) noexcept = default;
    ~OrientedGraph() = default;

    const_iterator begin() const { return const_iterator(graph_.begin()); }
    const_iterator end() const { return const_iterator(graph_.end()); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    void addVertex(const_reference value) {
        if (contains(value))
            throw VertexAlreadyExistsException();
        graph_.emplace_back(value);
    }
    void addEdge(const_reference from, const_reference to) {
        const difference_type fromIndex = getIndex(from);
        const difference_type toIndex = getIndex(to);
        if (fromIndex == unknownIndex || toIndex == unknownIndex)
            throw NoSuchVertexException();
        vertex_type &source = graph_[static_cast<size_type>(fromIndex)];
        const edge_type target = static_cast<edge_type>(toIndex);
        if (source.hasEdge(target))
            throw EdgeAlreadyExistsException();
        source.addEdge(target);
    }
    const_reference at(size_type index) const {
        if (index >= graph_.size())
            throw IndexOutOfRangeException();
        return graph_[index].getValue();
    }
    edges_type getEdges(const_reference vertex) const {
        const difference_type vertexIndex = getIndex(vertex);
        if (vertexIndex == unknownIndex)
            throw NoSuchVertexException();
        return graph_[static_cast<size_type>(vertexIndex)].getEdges();
    }

    bool contains(const_reference value) const { return getIndex(value) != unknownIndex; }
    size_type size() const { return graph_.size(); }
    bool empty() const { return graph_.empty(); }
    void clear() { graph_.clear(); }
    void removeEdge(const_reference from, const_reference to) {
        const difference_type fromIndex = getIndex(from);
        const difference_type toIndex = getIndex(to);
        if (fromIndex == unknownIndex || toIndex == unknownIndex)
            throw NoSuchVertexException();
        vertex_type &source = graph_[static_cast<size_type>(fromIndex)];
        const edge_type target = static_cast<edge_type>(toIndex);
        if (!source.hasEdge(target))
            throw NoSuchEdgeException();
        source.removeEdge(target);
    }
    void removeVertex(const_reference vertex) {
        const difference_type vertexIndex = getIndex(vertex);
        if (vertexIndex == unknownIndex)
            throw NoSuchVertexException();
        const size_type removed = static_cast<size_type>(vertexIndex);
        for (size_type i = 0; i < graph_.size(); ++i) {
            if (i != removed)
                graph_[i].reindexEdgesAfterRemoval(static_cast<edge_type>(removed));
        }
        graph_.erase(graph_.begin() + static_cast<difference_type>(removed));
    }

    bool operator==(const OrientedGraph &other) const { return graph_ == other.graph_; }
    bool operator!=(const OrientedGraph &other) const { return !(*this == other); }
    bool operator<(const OrientedGraph &other) const { return graph_ < other.graph_; }
    bool operator>(const OrientedGraph &other) const { return other < *this; }
    bool operator<=(const OrientedGraph &other) const { return !(other < *this); }
    bool operator>=(const OrientedGraph &other) const { return !(*this < other); }

    private:
    difference_type getIndex(const_reference value) const {
        for (size_type i = 0; i < graph_.size(); ++i) {
            if (graph_[i].getValue() == value)
                return static_cast<difference_type>(i);
        }
        return unknownIndex;
    }
    container_type graph_;
};

template <typename T> std::ostream &operator<<(std::ostream &os, const OrientedGraph<T> &graph) {
    os << '{';
    bool firstVertex = true;
    std::for_each(graph.cbegin(), graph.cend(), [&](const T &value) {
        if (firstVertex)
            firstVertex = false;
        else
            os << ", ";
        os << value << " -> [";
        bool firstEdge = true;
        const auto edges = graph.getEdges(value);
        for (const auto &edge : edges) {
            if (firstEdge)
                firstEdge = false;
            else
                os << ' ';
            os << graph.at(static_cast<std::size_t>(edge));
        }
        os << ']';
    });
    os << '}';
    return os;
}
