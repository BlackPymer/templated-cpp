#pragma once
#include "exceptions.hpp"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T> class OrientedGraph {
    public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using edge_type = std::pair<value_type, value_type>;
    using container_type = std::vector<value_type>;
    using matrix_type = std::vector<std::vector<bool>>;

    template <bool Const> class VertexIterator {
        public:
        using owner_type = std::conditional_t<Const, const OrientedGraph, OrientedGraph>;
        using graph_pointer = owner_type *;
        using value_type = typename OrientedGraph::value_type;
        using reference = std::conditional_t<Const, const value_type &, value_type &>;
        using pointer = std::conditional_t<Const, const value_type *, value_type *>;
        using difference_type = typename OrientedGraph::difference_type;
        using iterator_category = std::bidirectional_iterator_tag;

        VertexIterator() noexcept : graph(nullptr), index(0) {}
        VertexIterator(graph_pointer owner, size_type index) noexcept : graph(owner), index(index) {}
        template <bool Other, typename = std::enable_if_t<Const && !Other>>
        VertexIterator(const VertexIterator<Other> &other) noexcept
            : graph(other.graph), index(other.index) {}

        reference operator*() const { return graph->vertices_[index]; }
        pointer operator->() const { return &graph->vertices_[index]; }
        VertexIterator &operator++() {
            ++index;
            return *this;
        }
        VertexIterator operator++(int) {
            VertexIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        VertexIterator &operator--() {
            --index;
            return *this;
        }
        VertexIterator operator--(int) {
            VertexIterator tmp = *this;
            --(*this);
            return tmp;
        }
        bool operator==(const VertexIterator &other) const {
            return graph == other.graph && index == other.index;
        }
        bool operator!=(const VertexIterator &other) const { return !(*this == other); }

        size_type base() const noexcept { return index; }
        graph_pointer owner() const noexcept { return graph; }

        private:
        template <bool> friend class VertexIterator;
        graph_pointer graph;
        size_type index;
    };

    template <bool Const> class EdgeIterator {
        public:
        using owner_type = std::conditional_t<Const, const OrientedGraph, OrientedGraph>;
        using graph_pointer = owner_type *;
        using value_type = typename OrientedGraph::edge_type;
        using reference = value_type;
        using pointer = void;
        using difference_type = typename OrientedGraph::difference_type;
        using iterator_category = std::bidirectional_iterator_tag;

        EdgeIterator() noexcept : graph(nullptr), position(0) {}
        EdgeIterator(graph_pointer owner, size_type position) noexcept
            : graph(owner), position(position) {
            settleForward();
        }
        template <bool Other, typename = std::enable_if_t<Const && !Other>>
        EdgeIterator(const EdgeIterator<Other> &other) noexcept
            : graph(other.graph), position(other.position) {}

        reference operator*() const {
            const size_type side = graph->size();
            return edge_type(graph->vertices_[position / side], graph->vertices_[position % side]);
        }
        EdgeIterator &operator++() {
            ++position;
            settleForward();
            return *this;
        }
        EdgeIterator operator++(int) {
            EdgeIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        EdgeIterator &operator--() {
            settleBackward();
            return *this;
        }
        EdgeIterator operator--(int) {
            EdgeIterator tmp = *this;
            --(*this);
            return tmp;
        }
        bool operator==(const EdgeIterator &other) const {
            return graph == other.graph && position == other.position;
        }
        bool operator!=(const EdgeIterator &other) const { return !(*this == other); }

        size_type base() const noexcept { return position; }
        graph_pointer owner() const noexcept { return graph; }

        private:
        template <bool> friend class EdgeIterator;
        size_type total() const { return graph->size() * graph->size(); }
        bool cell(size_type offset) const {
            const size_type side = graph->size();
            return graph->matrix_[offset / side][offset % side];
        }
        void settleForward() {
            while (position < total() && !cell(position))
                ++position;
        }
        void settleBackward() {
            while (position > 0) {
                --position;
                if (cell(position))
                    return;
            }
            position = total();
        }
        graph_pointer graph;
        size_type position;
    };

    template <bool Const> class NeighborIterator {
        public:
        using owner_type = std::conditional_t<Const, const OrientedGraph, OrientedGraph>;
        using graph_pointer = owner_type *;
        using value_type = typename OrientedGraph::value_type;
        using reference = std::conditional_t<Const, const value_type &, value_type &>;
        using pointer = std::conditional_t<Const, const value_type *, value_type *>;
        using difference_type = typename OrientedGraph::difference_type;
        using iterator_category = std::bidirectional_iterator_tag;

        NeighborIterator() noexcept : graph(nullptr), vertex(0), column(0) {}
        NeighborIterator(graph_pointer owner, size_type vertex, size_type column) noexcept
            : graph(owner), vertex(vertex), column(column) {
            settleForward();
        }
        template <bool Other, typename = std::enable_if_t<Const && !Other>>
        NeighborIterator(const NeighborIterator<Other> &other) noexcept
            : graph(other.graph), vertex(other.vertex), column(other.column) {}

        reference operator*() const { return graph->vertices_[column]; }
        pointer operator->() const { return &graph->vertices_[column]; }
        NeighborIterator &operator++() {
            ++column;
            settleForward();
            return *this;
        }
        NeighborIterator operator++(int) {
            NeighborIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        NeighborIterator &operator--() {
            settleBackward();
            return *this;
        }
        NeighborIterator operator--(int) {
            NeighborIterator tmp = *this;
            --(*this);
            return tmp;
        }
        bool operator==(const NeighborIterator &other) const {
            return graph == other.graph && vertex == other.vertex && column == other.column;
        }
        bool operator!=(const NeighborIterator &other) const { return !(*this == other); }

        size_type base() const noexcept { return column; }
        graph_pointer owner() const noexcept { return graph; }

        private:
        template <bool> friend class NeighborIterator;
        bool cell(size_type offset) const { return graph->matrix_[vertex][offset]; }
        void settleForward() {
            while (column < graph->size() && !cell(column))
                ++column;
        }
        void settleBackward() {
            while (column > 0) {
                --column;
                if (cell(column))
                    return;
            }
            column = graph->size();
        }
        graph_pointer graph;
        size_type vertex;
        size_type column;
    };

    template <bool Const> class IncidentIterator {
        public:
        using owner_type = std::conditional_t<Const, const OrientedGraph, OrientedGraph>;
        using graph_pointer = owner_type *;
        using value_type = typename OrientedGraph::edge_type;
        using reference = value_type;
        using pointer = void;
        using difference_type = typename OrientedGraph::difference_type;
        using iterator_category = std::bidirectional_iterator_tag;

        IncidentIterator() noexcept : graph(nullptr), vertex(0), position(0) {}
        IncidentIterator(graph_pointer owner, size_type vertex, size_type position) noexcept
            : graph(owner), vertex(vertex), position(position) {
            settleForward();
        }
        template <bool Other, typename = std::enable_if_t<Const && !Other>>
        IncidentIterator(const IncidentIterator<Other> &other) noexcept
            : graph(other.graph), vertex(other.vertex), position(other.position) {}

        reference operator*() const {
            const size_type side = graph->size();
            if (position < side)
                return edge_type(graph->vertices_[vertex], graph->vertices_[position]);
            return edge_type(graph->vertices_[position - side], graph->vertices_[vertex]);
        }
        IncidentIterator &operator++() {
            ++position;
            settleForward();
            return *this;
        }
        IncidentIterator operator++(int) {
            IncidentIterator tmp = *this;
            ++(*this);
            return tmp;
        }
        IncidentIterator &operator--() {
            settleBackward();
            return *this;
        }
        IncidentIterator operator--(int) {
            IncidentIterator tmp = *this;
            --(*this);
            return tmp;
        }
        bool operator==(const IncidentIterator &other) const {
            return graph == other.graph && vertex == other.vertex && position == other.position;
        }
        bool operator!=(const IncidentIterator &other) const { return !(*this == other); }

        size_type base() const noexcept { return position; }
        graph_pointer owner() const noexcept { return graph; }

        private:
        template <bool> friend class IncidentIterator;
        size_type total() const { return 2 * graph->size(); }
        bool cell(size_type offset) const {
            const size_type side = graph->size();
            if (offset < side)
                return graph->matrix_[vertex][offset];
            const size_type source = offset - side;
            return source != vertex && graph->matrix_[source][vertex];
        }
        void settleForward() {
            while (position < total() && !cell(position))
                ++position;
        }
        void settleBackward() {
            while (position > 0) {
                --position;
                if (cell(position))
                    return;
            }
            position = total();
        }
        graph_pointer graph;
        size_type vertex;
        size_type position;
    };

    using iterator = VertexIterator<false>;
    using const_iterator = VertexIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using edge_iterator = EdgeIterator<false>;
    using const_edge_iterator = EdgeIterator<true>;
    using edge_reverse_iterator = std::reverse_iterator<edge_iterator>;
    using const_edge_reverse_iterator = std::reverse_iterator<const_edge_iterator>;

    using neighbor_iterator = NeighborIterator<false>;
    using const_neighbor_iterator = NeighborIterator<true>;
    using neighbor_reverse_iterator = std::reverse_iterator<neighbor_iterator>;
    using const_neighbor_reverse_iterator = std::reverse_iterator<const_neighbor_iterator>;

    using incident_iterator = IncidentIterator<false>;
    using const_incident_iterator = IncidentIterator<true>;
    using incident_reverse_iterator = std::reverse_iterator<incident_iterator>;
    using const_incident_reverse_iterator = std::reverse_iterator<const_incident_iterator>;

    OrientedGraph() = default;
    OrientedGraph(const OrientedGraph &) = default;
    OrientedGraph(OrientedGraph &&) noexcept = default;
    OrientedGraph &operator=(const OrientedGraph &) = default;
    OrientedGraph &operator=(OrientedGraph &&) noexcept = default;
    ~OrientedGraph() = default;

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, size()); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end() const { return const_iterator(this, size()); }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    edge_iterator edgeBegin() { return edge_iterator(this, 0); }
    edge_iterator edgeEnd() { return edge_iterator(this, size() * size()); }
    const_edge_iterator edgeBegin() const { return const_edge_iterator(this, 0); }
    const_edge_iterator edgeEnd() const { return const_edge_iterator(this, size() * size()); }
    edge_reverse_iterator edgeRBegin() { return edge_reverse_iterator(edgeEnd()); }
    edge_reverse_iterator edgeREnd() { return edge_reverse_iterator(edgeBegin()); }
    const_edge_reverse_iterator edgeRBegin() const {
        return const_edge_reverse_iterator(edgeEnd());
    }
    const_edge_reverse_iterator edgeREnd() const {
        return const_edge_reverse_iterator(edgeBegin());
    }

    neighbor_iterator neighborBegin(const_reference vertex) {
        return neighbor_iterator(this, indexOf(vertex), 0);
    }
    neighbor_iterator neighborEnd(const_reference vertex) {
        return neighbor_iterator(this, indexOf(vertex), size());
    }
    const_neighbor_iterator neighborBegin(const_reference vertex) const {
        return const_neighbor_iterator(this, indexOf(vertex), 0);
    }
    const_neighbor_iterator neighborEnd(const_reference vertex) const {
        return const_neighbor_iterator(this, indexOf(vertex), size());
    }
    neighbor_reverse_iterator neighborRBegin(const_reference vertex) {
        return neighbor_reverse_iterator(neighborEnd(vertex));
    }
    neighbor_reverse_iterator neighborREnd(const_reference vertex) {
        return neighbor_reverse_iterator(neighborBegin(vertex));
    }
    const_neighbor_reverse_iterator neighborRBegin(const_reference vertex) const {
        return const_neighbor_reverse_iterator(neighborEnd(vertex));
    }
    const_neighbor_reverse_iterator neighborREnd(const_reference vertex) const {
        return const_neighbor_reverse_iterator(neighborBegin(vertex));
    }

    incident_iterator incidentBegin(const_reference vertex) {
        return incident_iterator(this, indexOf(vertex), 0);
    }
    incident_iterator incidentEnd(const_reference vertex) {
        return incident_iterator(this, indexOf(vertex), 2 * size());
    }
    const_incident_iterator incidentBegin(const_reference vertex) const {
        return const_incident_iterator(this, indexOf(vertex), 0);
    }
    const_incident_iterator incidentEnd(const_reference vertex) const {
        return const_incident_iterator(this, indexOf(vertex), 2 * size());
    }
    incident_reverse_iterator incidentRBegin(const_reference vertex) {
        return incident_reverse_iterator(incidentEnd(vertex));
    }
    incident_reverse_iterator incidentREnd(const_reference vertex) {
        return incident_reverse_iterator(incidentBegin(vertex));
    }
    const_incident_reverse_iterator incidentRBegin(const_reference vertex) const {
        return const_incident_reverse_iterator(incidentEnd(vertex));
    }
    const_incident_reverse_iterator incidentREnd(const_reference vertex) const {
        return const_incident_reverse_iterator(incidentBegin(vertex));
    }

    bool contains(const_reference value) const {
        return std::find(vertices_.begin(), vertices_.end(), value) != vertices_.end();
    }
    size_type indexOf(const_reference value) const {
        const auto found = std::find(vertices_.begin(), vertices_.end(), value);
        if (found == vertices_.end())
            throw NoSuchVertexException();
        return static_cast<size_type>(std::distance(vertices_.begin(), found));
    }
    const_reference at(size_type index) const {
        if (index >= size())
            throw IndexOutOfRangeException();
        return vertices_[index];
    }
    reference at(size_type index) {
        if (index >= size())
            throw IndexOutOfRangeException();
        return vertices_[index];
    }

    size_type size() const { return vertices_.size(); }
    bool empty() const { return vertices_.empty(); }
    size_type edgeCount() const {
        size_type count = 0;
        for (size_type i = 0; i < size(); ++i)
            count += static_cast<size_type>(std::count(matrix_[i].begin(), matrix_[i].end(), true));
        return count;
    }
    bool hasEdge(const_reference from, const_reference to) const {
        return matrix_[indexOf(from)][indexOf(to)];
    }
    size_type degree(const_reference vertex) const {
        const size_type index = indexOf(vertex);
        size_type result = 0;
        for (size_type i = 0; i < size(); ++i) {
            if (matrix_[index][i])
                ++result;
            if (matrix_[i][index])
                ++result;
        }
        return result;
    }
    size_type edgeDegree(const_reference from, const_reference to) const {
        if (!hasEdge(from, to))
            throw NoSuchEdgeException();
        return from == to ? 1 : 2;
    }

    void addVertex(const_reference value) {
        if (contains(value))
            throw VertexAlreadyExistsException();
        vertices_.push_back(value);
        const size_type count = size();
        for (size_type i = 0; i + 1 < count; ++i)
            matrix_[i].push_back(false);
        matrix_.emplace_back(count, false);
    }
    void addEdge(const_reference from, const_reference to) {
        const size_type source = indexOf(from);
        const size_type target = indexOf(to);
        if (matrix_[source][target])
            throw EdgeAlreadyExistsException();
        matrix_[source][target] = true;
    }
    void removeEdge(const_reference from, const_reference to) {
        const size_type source = indexOf(from);
        const size_type target = indexOf(to);
        if (!matrix_[source][target])
            throw NoSuchEdgeException();
        matrix_[source][target] = false;
    }
    void removeVertex(const_reference value) { erase(const_iterator(this, indexOf(value))); }
    void clear() {
        vertices_.clear();
        matrix_.clear();
    }

    iterator erase(const_iterator position) {
        const size_type index = position.base();
        if (index >= size())
            throw IndexOutOfRangeException();
        matrix_.erase(matrix_.begin() + static_cast<difference_type>(index));
        for (size_type i = 0; i < matrix_.size(); ++i)
            matrix_[i].erase(matrix_[i].begin() + static_cast<difference_type>(index));
        vertices_.erase(vertices_.begin() + static_cast<difference_type>(index));
        return iterator(this, index);
    }
    edge_iterator erase(const_edge_iterator position) {
        const size_type offset = position.base();
        const size_type side = size();
        if (offset >= side * side)
            throw IndexOutOfRangeException();
        matrix_[offset / side][offset % side] = false;
        return edge_iterator(this, offset);
    }

    bool operator==(const OrientedGraph &other) const {
        return vertices_ == other.vertices_ && matrix_ == other.matrix_;
    }
    bool operator!=(const OrientedGraph &other) const { return !(*this == other); }
    bool operator<(const OrientedGraph &other) const {
        if (vertices_ != other.vertices_)
            return vertices_ < other.vertices_;
        return matrix_ < other.matrix_;
    }
    bool operator>(const OrientedGraph &other) const { return other < *this; }
    bool operator<=(const OrientedGraph &other) const { return !(other < *this); }
    bool operator>=(const OrientedGraph &other) const { return !(*this < other); }

    private:
    container_type vertices_;
    matrix_type matrix_;
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
        std::for_each(graph.neighborBegin(value), graph.neighborEnd(value), [&](const T &neighbor) {
            if (firstEdge)
                firstEdge = false;
            else
                os << ' ';
            os << neighbor;
        });
        os << ']';
    });
    os << '}';
    return os;
}
