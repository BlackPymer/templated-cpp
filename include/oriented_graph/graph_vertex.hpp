#pragma once
#include <utility>
#include <vector>

template <typename T> class GraphVertex {
    public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using edge_type = int;
    using edges_type = std::vector<edge_type>;
    using size_type = typename edges_type::size_type;
    using difference_type = typename edges_type::difference_type;

    explicit GraphVertex(value_type value) : value(value) {}
    GraphVertex(const GraphVertex &other) = default;

    const_reference getValue() const { return value; }
    void setValue(value_type newValue) { value = std::move(newValue); }

    const edges_type &getEdges() const { return edges; }
    size_type getEdgeCount() const { return edges.size(); }
    bool hasEdge(edge_type edge) const;
    void addEdge(edge_type edge) { edges.push_back(edge); }
    void removeEdge(edge_type edge);
    void reindexEdgesAfterRemoval(edge_type removedIndex);

    bool isEmpty() const { return edges.empty(); }

    bool operator==(const GraphVertex &other) const {
        return value == other.value && edges == other.edges;
    }
    bool operator!=(const GraphVertex &other) const { return !(*this == other); }
    bool operator<(const GraphVertex &other) const {
        if (!(value == other.value))
            return value < other.value;
        return edges < other.edges;
    }
    bool operator>(const GraphVertex &other) const { return other < *this; }
    bool operator<=(const GraphVertex &other) const { return !(other < *this); }
    bool operator>=(const GraphVertex &other) const { return !(*this < other); }

    private:
    edges_type edges;
    value_type value;
};

template <typename T> bool GraphVertex<T>::hasEdge(edge_type edge) const {
    for (size_type i = 0; i < edges.size(); ++i) {
        if (edges[i] == edge)
            return true;
    }
    return false;
}

template <typename T> void GraphVertex<T>::removeEdge(edge_type edge) {
    for (size_type i = 0; i < edges.size(); ++i) {
        if (edges[i] == edge) {
            edges.erase(edges.begin() + static_cast<difference_type>(i));
            return;
        }
    }
}

template <typename T> void GraphVertex<T>::reindexEdgesAfterRemoval(edge_type removedIndex) {
    size_type i = 0;
    while (i < edges.size()) {
        if (edges[i] == removedIndex) {
            edges.erase(edges.begin() + static_cast<difference_type>(i));
            continue;
        }
        if (edges[i] > removedIndex)
            --edges[i];
        ++i;
    }
}
