#include <UnitTest++/UnitTest++.h>

#include "oriented_graph/oriented_graph.hpp"

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

using Graph = OrientedGraph<std::string>;
using edge_type = Graph::edge_type;

Graph makeGraph() {
    Graph graph;
    graph.addVertex("a");
    graph.addVertex("b");
    graph.addVertex("c");
    graph.addVertex("d");
    graph.addEdge("a", "b");
    graph.addEdge("a", "c");
    graph.addEdge("c", "a");
    graph.addEdge("d", "a");
    graph.addEdge("b", "d");
    return graph;
}

template <typename Iterator>
std::vector<std::string> collectValues(Iterator begin, Iterator end) {
    std::vector<std::string> result;
    for (auto it = begin; it != end; ++it)
        result.push_back(*it);
    return result;
}

template <typename Iterator>
std::vector<std::string> collectValuesBack(Iterator begin, Iterator end) {
    std::vector<std::string> result;
    auto it = end;
    while (it != begin) {
        --it;
        result.push_back(*it);
    }
    return result;
}

template <typename Iterator> std::vector<edge_type> collectEdges(Iterator begin, Iterator end) {
    std::vector<edge_type> result;
    for (auto it = begin; it != end; ++it)
        result.push_back(*it);
    return result;
}

template <typename Iterator> std::vector<edge_type> collectEdgesBack(Iterator begin, Iterator end) {
    std::vector<edge_type> result;
    auto it = end;
    while (it != begin) {
        --it;
        result.push_back(*it);
    }
    return result;
}

std::vector<std::string> neighbors(const Graph &graph, const std::string &vertex) {
    return collectValues(graph.neighborBegin(vertex), graph.neighborEnd(vertex));
}

std::vector<edge_type> incident(const Graph &graph, const std::string &vertex) {
    return collectEdges(graph.incidentBegin(vertex), graph.incidentEnd(vertex));
}

} // namespace

SUITE(GraphTypeInfo) {
    TEST(PublicTypeAliases) {
        static_assert(std::is_same_v<Graph::value_type, std::string>);
        static_assert(std::is_same_v<Graph::const_reference, const std::string &>);
        static_assert(std::is_same_v<Graph::reference, std::string &>);
        static_assert(std::is_same_v<Graph::size_type, std::size_t>);
        static_assert(std::is_same_v<Graph::edge_type, std::pair<std::string, std::string>>);
        static_assert(std::is_same_v<Graph::container_type, std::vector<std::string>>);
        static_assert(std::is_same_v<Graph::iterator, Graph::VertexIterator<false>>);
        static_assert(std::is_same_v<Graph::const_iterator, Graph::VertexIterator<true>>);
        static_assert(std::is_same_v<Graph::reverse_iterator, std::reverse_iterator<Graph::iterator>>);
        static_assert(std::is_same_v<Graph::const_edge_iterator, Graph::EdgeIterator<true>>);
        static_assert(std::is_same_v<Graph::neighbor_iterator, Graph::NeighborIterator<false>>);
        static_assert(
            std::is_same_v<Graph::const_incident_iterator, Graph::IncidentIterator<true>>);
        static_assert(
            std::is_same_v<Graph::const_edge_reverse_iterator,
                           std::reverse_iterator<Graph::const_edge_iterator>>);
        CHECK(true);
    }

    TEST(IteratorsAreBidirectional) {
        static_assert(std::is_same_v<Graph::iterator::iterator_category, std::bidirectional_iterator_tag>);
        static_assert(std::is_same_v<Graph::edge_iterator::iterator_category,
                                     std::bidirectional_iterator_tag>);
        static_assert(std::is_same_v<Graph::neighbor_iterator::iterator_category,
                                     std::bidirectional_iterator_tag>);
        static_assert(std::is_same_v<Graph::incident_iterator::iterator_category,
                                     std::bidirectional_iterator_tag>);
        CHECK(true);
    }
}

SUITE(GraphLifecycle) {
    TEST(DefaultConstructorGivesEmptyContainer) {
        Graph graph;
        CHECK(graph.empty());
        CHECK(graph.size() == 0u);
        CHECK(graph.begin() == graph.end());
        CHECK(graph.edgeBegin() == graph.edgeEnd());
    }

    TEST(CopyConstructorMakesDeepCopy) {
        Graph graph = makeGraph();
        Graph copy = graph;
        CHECK(copy == graph);
        copy.addVertex("z");
        CHECK(copy != graph);
        CHECK(graph.size() == 4u);
    }

    TEST(CopyAssignmentMakesDeepCopy) {
        Graph graph = makeGraph();
        Graph assigned;
        assigned.addVertex("single");
        assigned = graph;
        CHECK(assigned == graph);
        assigned.removeVertex("a");
        CHECK(assigned.size() == 3u);
        CHECK(graph.size() == 4u);
    }

    TEST(MoveConstructorKeepsContent) {
        Graph graph = makeGraph();
        Graph moved = std::move(graph);
        CHECK(moved.size() == 4u);
        CHECK(moved.edgeCount() == 5u);
    }

    TEST(MoveAssignmentKeepsContent) {
        Graph graph = makeGraph();
        Graph moved;
        moved = std::move(graph);
        CHECK(moved.edgeCount() == 5u);
        CHECK(moved.hasEdge("a", "b"));
    }

    TEST(ClearEmptiesContainer) {
        Graph graph = makeGraph();
        graph.clear();
        CHECK(graph.empty());
        CHECK(graph.size() == 0u);
        CHECK(graph.edgeCount() == 0u);
    }
}

SUITE(GraphVertices) {
    TEST(AddVertexAndCheckPresence) {
        Graph graph;
        graph.addVertex("a");
        graph.addVertex("b");
        CHECK(graph.contains("a"));
        CHECK(graph.contains("b"));
        CHECK(!graph.contains("c"));
        CHECK(graph.size() == 2u);
    }

    TEST(AddingExistingVertexThrows) {
        Graph graph;
        graph.addVertex("a");
        CHECK_THROW(graph.addVertex("a"), VertexAlreadyExistsException);
        CHECK_THROW(graph.addVertex("a"), GraphException);
        CHECK(graph.size() == 1u);
    }

    TEST(IndexOfAndAtGiveAccessToElements) {
        Graph graph = makeGraph();
        CHECK(graph.indexOf("a") == 0u);
        CHECK(graph.indexOf("c") == 2u);
        CHECK(graph.at(0) == "a");
        CHECK(graph.at(3) == "d");
        CHECK_THROW(graph.indexOf("missing"), NoSuchVertexException);
        CHECK_THROW(graph.at(4), IndexOutOfRangeException);
        CHECK_THROW(graph.at(99), IndexOutOfRangeException);
    }

    TEST(AtGivesMutableReference) {
        Graph graph = makeGraph();
        graph.at(1) = "b2";
        CHECK(graph.contains("b2"));
        CHECK(!graph.contains("b"));
        CHECK(graph.hasEdge("a", "b2"));
    }

    TEST(RemoveVertexByValue) {
        Graph graph = makeGraph();
        graph.removeVertex("b");
        CHECK(!graph.contains("b"));
        CHECK(graph.size() == 3u);
        CHECK(graph.edgeCount() == 3u);
        CHECK(neighbors(graph, "a") == std::vector<std::string>({"c"}));
        CHECK(neighbors(graph, "d") == std::vector<std::string>({"a"}));
        CHECK_THROW(graph.removeVertex("b"), NoSuchVertexException);
    }

    TEST(RemoveFirstVertexReindexesEdges) {
        Graph graph = makeGraph();
        graph.removeVertex("a");
        CHECK(graph.size() == 3u);
        CHECK(graph.edgeCount() == 2u);
        CHECK(neighbors(graph, "b") == std::vector<std::string>({"d"}));
        CHECK(neighbors(graph, "c") == std::vector<std::string>());
        CHECK(neighbors(graph, "d") == std::vector<std::string>());
    }

    TEST(RemoveVertexByIteratorReturnsNext) {
        Graph graph = makeGraph();
        Graph::iterator next = graph.erase(Graph::const_iterator(&graph, 1));
        CHECK(*next == "c");
        CHECK(graph.size() == 3u);
        CHECK(!graph.contains("b"));
    }

    TEST(RemoveLastVertexByIteratorGivesEnd) {
        Graph graph = makeGraph();
        Graph::iterator next = graph.erase(Graph::const_iterator(&graph, 3));
        CHECK(next == graph.end());
        CHECK(graph.size() == 3u);
    }

    TEST(RemoveEndByIteratorThrows) {
        Graph graph = makeGraph();
        CHECK_THROW(graph.erase(graph.end()), IndexOutOfRangeException);
        CHECK(graph.size() == 4u);
    }

    TEST(VertexIterationForwardAndBackward) {
        const Graph graph = makeGraph();
        const std::vector<std::string> expected = {"a", "b", "c", "d"};
        const std::vector<std::string> reversed = {"d", "c", "b", "a"};
        CHECK(collectValues(graph.begin(), graph.end()) == expected);
        CHECK(collectValues(graph.cbegin(), graph.cend()) == expected);
        CHECK(collectValuesBack(graph.begin(), graph.end()) == reversed);
    }

    TEST(VertexReverseIteration) {
        const Graph graph = makeGraph();
        const std::vector<std::string> reversed = {"d", "c", "b", "a"};
        CHECK(collectValues(graph.rbegin(), graph.rend()) == reversed);
        CHECK(collectValues(graph.crbegin(), graph.crend()) == reversed);
        CHECK(graph.crbegin() != graph.crend());
        CHECK(collectValuesBack(graph.rbegin(), graph.rend()) ==
              std::vector<std::string>({"a", "b", "c", "d"}));
    }

    TEST(VertexIteratorPostfixOperators) {
        Graph graph = makeGraph();
        Graph::iterator it = graph.begin();
        CHECK(*(it++) == "a");
        CHECK(*it == "b");
        Graph::iterator previous = it--;
        CHECK(*previous == "b");
        CHECK(*it == "a");
        Graph::const_iterator constIt = graph.cbegin();
        CHECK(*(constIt++) == "a");
        constIt--;
        CHECK(*constIt == "a");
    }

    TEST(VertexIteratorConversionToConst) {
        Graph graph = makeGraph();
        Graph::iterator mutableIt = graph.begin();
        Graph::const_iterator constIt = mutableIt;
        ++mutableIt;
        CHECK(constIt != mutableIt);
        CHECK(constIt == graph.cbegin());
        Graph::reverse_iterator reverseIt(graph.end());
        Graph::const_reverse_iterator constReverse = reverseIt;
        CHECK(constReverse == Graph::const_reverse_iterator(graph.end()));
    }

    TEST(VertexIteratorMemberAccess) {
        Graph graph = makeGraph();
        Graph::iterator it = graph.begin();
        CHECK(it->size() == 1u);
        *it = "z";
        CHECK(*it == "z");
        Graph::const_iterator constIt = graph.cbegin();
        CHECK(constIt->size() == 1u);
        CHECK(constIt.base() == 0u);
        CHECK(constIt.owner() == &graph);
    }

    TEST(DefaultConstructedIteratorsAreEqual) {
        Graph::iterator first;
        Graph::iterator second;
        CHECK(first == second);
        Graph::edge_iterator edgeFirst;
        Graph::edge_iterator edgeSecond;
        CHECK(edgeFirst == edgeSecond);
        CHECK(edgeFirst.base() == 0u);
        CHECK(edgeFirst.owner() == nullptr);
        Graph::neighbor_iterator neighborFirst;
        Graph::neighbor_iterator neighborSecond;
        CHECK(neighborFirst == neighborSecond);
        Graph::incident_iterator incidentFirst;
        Graph::incident_iterator incidentSecond;
        CHECK(incidentFirst == incidentSecond);
    }
}

SUITE(GraphEdges) {
    TEST(AddEdgeAndCheckPresence) {
        Graph graph;
        graph.addVertex("a");
        graph.addVertex("b");
        CHECK(!graph.hasEdge("a", "b"));
        graph.addEdge("a", "b");
        CHECK(graph.hasEdge("a", "b"));
        CHECK(!graph.hasEdge("b", "a"));
        CHECK(graph.edgeCount() == 1u);
    }

    TEST(AddEdgeForMissingVertexThrows) {
        Graph graph;
        graph.addVertex("a");
        CHECK_THROW(graph.addEdge("a", "b"), NoSuchVertexException);
        CHECK_THROW(graph.addEdge("b", "a"), NoSuchVertexException);
        CHECK_THROW(graph.hasEdge("a", "b"), NoSuchVertexException);
        CHECK(graph.edgeCount() == 0u);
    }

    TEST(AddDuplicateEdgeThrows) {
        Graph graph;
        graph.addVertex("a");
        graph.addVertex("b");
        graph.addEdge("a", "b");
        CHECK_THROW(graph.addEdge("a", "b"), EdgeAlreadyExistsException);
        CHECK_THROW(graph.addEdge("a", "b"), GraphException);
        CHECK(graph.edgeCount() == 1u);
    }

    TEST(RemoveEdge) {
        Graph graph = makeGraph();
        graph.removeEdge("a", "b");
        CHECK(!graph.hasEdge("a", "b"));
        CHECK(graph.edgeCount() == 4u);
        CHECK_THROW(graph.removeEdge("a", "b"), NoSuchEdgeException);
        CHECK_THROW(graph.removeEdge("b", "a"), NoSuchVertexException);
        CHECK_THROW(graph.edgeDegree("a", "b"), NoSuchEdgeException);
    }

    TEST(SelfLoopIsSupported) {
        Graph graph;
        graph.addVertex("x");
        graph.addEdge("x", "x");
        CHECK(graph.hasEdge("x", "x"));
        CHECK(graph.edgeCount() == 1u);
        CHECK(graph.degree("x") == 2u);
        CHECK(graph.edgeDegree("x", "x") == 1u);
        CHECK(incident(graph, "x") == std::vector<edge_type>({{"x", "x"}}));
        CHECK(neighbors(graph, "x") == std::vector<std::string>({"x"}));
    }

    TEST(EdgeIterationForwardAndBackward) {
        const Graph graph = makeGraph();
        const std::vector<edge_type> expected = {{"a", "b"},
                                                 {"a", "c"},
                                                 {"b", "d"},
                                                 {"c", "a"},
                                                 {"d", "a"}};
        CHECK(collectEdges(graph.edgeBegin(), graph.edgeEnd()) == expected);
        CHECK(collectEdgesBack(graph.edgeBegin(), graph.edgeEnd()) ==
              std::vector<edge_type>({{"d", "a"}, {"c", "a"}, {"b", "d"}, {"a", "c"}, {"a", "b"}}));
        CHECK(graph.edgeBegin() != graph.edgeEnd());
    }

    TEST(EdgeReverseIteration) {
        Graph graph = makeGraph();
        const std::vector<edge_type> reversed = {{"d", "a"}, {"c", "a"}, {"b", "d"}, {"a", "c"},
                                                 {"a", "b"}};
        CHECK(collectEdges(graph.edgeRBegin(), graph.edgeREnd()) == reversed);
        const Graph &constant = graph;
        CHECK(collectEdges(constant.edgeRBegin(), constant.edgeREnd()) == reversed);
        CHECK(collectEdgesBack(constant.edgeRBegin(), constant.edgeREnd()) ==
              std::vector<edge_type>({{"a", "b"}, {"a", "c"}, {"b", "d"}, {"c", "a"}, {"d", "a"}}));
    }

    TEST(EdgeIteratorPostfixOperators) {
        Graph graph = makeGraph();
        Graph::edge_iterator it = graph.edgeBegin();
        const edge_type first = *it;
        const edge_type afterPostfix = *(it++);
        CHECK(afterPostfix == first);
        CHECK(*it == edge_type("a", "c"));
        Graph::edge_iterator previous = it--;
        CHECK(*previous == edge_type("a", "c"));
        CHECK(*it == first);
        Graph::const_edge_iterator constIt = graph.edgeBegin();
        CHECK(*(constIt++) == first);
        constIt--;
        CHECK(*constIt == first);
        CHECK(constIt.base() == 0u);
        CHECK(constIt.owner() == &graph);
    }

    TEST(EdgeIteratorConversionToConst) {
        Graph graph = makeGraph();
        Graph::edge_iterator mutableIt = graph.edgeBegin();
        Graph::const_edge_iterator constIt = mutableIt;
        ++mutableIt;
        CHECK(constIt != mutableIt);
        CHECK(constIt == graph.edgeBegin());
        Graph::edge_reverse_iterator reverseIt(graph.edgeEnd());
        Graph::const_edge_reverse_iterator constReverse = reverseIt;
        CHECK(constReverse == Graph::const_edge_reverse_iterator(graph.edgeEnd()));
    }

    TEST(EraseEdgeByIteratorReturnsNext) {
        Graph graph;
        graph.addVertex("q");
        graph.addVertex("w");
        graph.addVertex("e");
        graph.addEdge("q", "w");
        graph.addEdge("q", "e");
        graph.addEdge("e", "q");

        Graph::edge_iterator next = graph.erase(Graph::const_edge_iterator(&graph, 1));
        CHECK(graph.edgeCount() == 2u);
        CHECK(!graph.hasEdge("q", "e"));
        CHECK(*next == edge_type("e", "q"));
        CHECK(next != graph.edgeEnd());
    }

    TEST(EraseLastEdgeByIteratorGivesEnd) {
        Graph graph;
        graph.addVertex("q");
        graph.addVertex("w");
        graph.addEdge("q", "w");
        Graph::edge_iterator next = graph.erase(Graph::const_edge_iterator(&graph, 0));
        CHECK(next == graph.edgeEnd());
        CHECK(graph.edgeCount() == 0u);
        CHECK_THROW(graph.erase(graph.edgeEnd()), IndexOutOfRangeException);
    }

    TEST(EraseEdgeOnEmptyGraphThrows) {
        Graph graph;
        graph.addVertex("only");
        CHECK_THROW(graph.erase(graph.edgeEnd()), IndexOutOfRangeException);
    }
}

SUITE(GraphDegrees) {
    TEST(EdgeCountCountsAllEdges) {
        Graph graph = makeGraph();
        CHECK(graph.edgeCount() == 5u);
        graph.addEdge("a", "d");
        CHECK(graph.edgeCount() == 6u);
        graph.removeEdge("a", "d");
        CHECK(graph.edgeCount() == 5u);
        graph.clear();
        CHECK(graph.edgeCount() == 0u);
    }

    TEST(VertexDegreeIsIncomingPlusOutgoing) {
        const Graph graph = makeGraph();
        CHECK(graph.degree("a") == 4u);
        CHECK(graph.degree("b") == 2u);
        CHECK(graph.degree("c") == 2u);
        CHECK(graph.degree("d") == 2u);
        CHECK_THROW(graph.degree("missing"), NoSuchVertexException);
    }

    TEST(DegreeOfVertexWithoutEdges) {
        Graph graph;
        graph.addVertex("lonely");
        graph.addVertex("other");
        CHECK(graph.degree("lonely") == 0u);
        CHECK(graph.edgeCount() == 0u);
    }

    TEST(EdgeDegreeCountsEndpoints) {
        const Graph graph = makeGraph();
        CHECK(graph.edgeDegree("a", "b") == 2u);
        CHECK(graph.edgeDegree("d", "a") == 2u);
        CHECK_THROW(graph.edgeDegree("b", "a"), NoSuchEdgeException);
        CHECK_THROW(graph.edgeDegree("missing", "a"), NoSuchVertexException);
    }
}

SUITE(GraphNeighbors) {
    TEST(NeighborIterationForward) {
        const Graph graph = makeGraph();
        CHECK(neighbors(graph, "a") == std::vector<std::string>({"b", "c"}));
        CHECK(neighbors(graph, "b") == std::vector<std::string>({"d"}));
        CHECK(neighbors(graph, "d") == std::vector<std::string>({"a"}));
    }

    TEST(NeighborWithoutEdgesGivesEmptyRange) {
        Graph graph;
        graph.addVertex("a");
        graph.addVertex("b");
        CHECK(neighbors(graph, "a") == std::vector<std::string>());
        CHECK(graph.neighborBegin("a") == graph.neighborEnd("a"));
        CHECK(collectValuesBack(graph.neighborBegin("a"), graph.neighborEnd("a")) ==
              std::vector<std::string>());
    }

    TEST(NeighborReverseIteration) {
        const Graph graph = makeGraph();
        CHECK(collectValuesBack(graph.neighborBegin("a"), graph.neighborEnd("a")) ==
              std::vector<std::string>({"c", "b"}));
        CHECK(collectValues(graph.neighborRBegin("a"), graph.neighborREnd("a")) ==
              std::vector<std::string>({"c", "b"}));
        const Graph &constant = graph;
        CHECK(collectValues(constant.neighborRBegin("a"), constant.neighborREnd("a")) ==
              std::vector<std::string>({"c", "b"}));
        CHECK(collectValuesBack(constant.neighborRBegin("a"), constant.neighborREnd("a")) ==
              std::vector<std::string>({"b", "c"}));
    }

    TEST(NeighborIteratorPostfixAndConversion) {
        Graph graph = makeGraph();
        Graph::neighbor_iterator it = graph.neighborBegin("a");
        CHECK(*(it++) == "b");
        CHECK(*it == "c");
        it--;
        CHECK(*it == "b");
        Graph::const_neighbor_iterator constIt = graph.neighborBegin("a");
        CHECK(*(constIt++) == "b");
        constIt--;
        CHECK(constIt == graph.neighborBegin("a"));
        CHECK(constIt.base() == 1u);
        Graph::neighbor_iterator mutableIt = graph.neighborBegin("a");
        Graph::const_neighbor_iterator converted = mutableIt;
        CHECK(converted == constIt);
        *mutableIt = "b2";
        CHECK(*mutableIt == "b2");
        Graph::neighbor_reverse_iterator reverseIt(graph.neighborEnd("a"));
        Graph::const_neighbor_reverse_iterator constReverse = reverseIt;
        CHECK(constReverse == Graph::const_neighbor_reverse_iterator(graph.neighborEnd("a")));
    }

    TEST(NeighborMissingVertexThrows) {
        Graph graph = makeGraph();
        CHECK_THROW(graph.neighborBegin("missing"), NoSuchVertexException);
        CHECK_THROW(graph.neighborEnd("missing"), NoSuchVertexException);
        CHECK_THROW(graph.neighborBegin("missing"), GraphException);
    }
}

SUITE(GraphIncident) {
    TEST(IncidentGivesOutgoingThenIncoming) {
        const Graph graph = makeGraph();
        const std::vector<edge_type> expected = {{"a", "b"}, {"a", "c"}, {"c", "a"}, {"d", "a"}};
        CHECK(incident(graph, "a") == expected);
        CHECK(incident(graph, "b") == std::vector<edge_type>({{"b", "d"}, {"a", "b"}}));
        CHECK(incident(graph, "c") == std::vector<edge_type>({{"c", "a"}, {"a", "c"}}));
    }

    TEST(IncidentWithoutEdgesIsEmpty) {
        Graph graph;
        graph.addVertex("a");
        graph.addVertex("b");
        CHECK(incident(graph, "a") == std::vector<edge_type>());
        CHECK(graph.incidentBegin("a") == graph.incidentEnd("a"));
    }

    TEST(IncidentReverseIteration) {
        const Graph graph = makeGraph();
        CHECK(collectEdgesBack(graph.incidentBegin("b"), graph.incidentEnd("b")) ==
              std::vector<edge_type>({{"a", "b"}, {"b", "d"}}));
        CHECK(collectEdges(graph.incidentRBegin("b"), graph.incidentREnd("b")) ==
              std::vector<edge_type>({{"a", "b"}, {"b", "d"}}));
        const Graph &constant = graph;
        CHECK(collectEdges(constant.incidentRBegin("b"), constant.incidentREnd("b")) ==
              std::vector<edge_type>({{"a", "b"}, {"b", "d"}}));
        CHECK(collectEdgesBack(constant.incidentRBegin("b"), constant.incidentREnd("b")) ==
              std::vector<edge_type>({{"b", "d"}, {"a", "b"}}));
    }

    TEST(IncidentIteratorPostfixAndConversion) {
        Graph graph = makeGraph();
        Graph::incident_iterator it = graph.incidentBegin("a");
        const edge_type first = *it;
        CHECK(*(it++) == first);
        CHECK(*it == edge_type("a", "c"));
        it--;
        CHECK(*it == first);
        Graph::const_incident_iterator constIt = graph.incidentBegin("a");
        CHECK(*(constIt++) == first);
        constIt--;
        CHECK(constIt == graph.incidentBegin("a"));
        CHECK(constIt.base() == 0u);
        CHECK(constIt.owner() == &graph);
        Graph::incident_iterator mutableIt = graph.incidentBegin("a");
        Graph::const_incident_iterator converted = mutableIt;
        CHECK(converted == constIt);
        Graph::incident_reverse_iterator reverseIt(graph.incidentEnd("a"));
        Graph::const_incident_reverse_iterator constReverse = reverseIt;
        CHECK(constReverse == Graph::const_incident_reverse_iterator(graph.incidentEnd("a")));
    }

    TEST(IncidentMissingVertexThrows) {
        Graph graph = makeGraph();
        CHECK_THROW(graph.incidentBegin("missing"), NoSuchVertexException);
        CHECK_THROW(graph.incidentEnd("missing"), NoSuchVertexException);
    }

    TEST(IncidentSelfLoopAppearsOnce) {
        Graph graph;
        graph.addVertex("x");
        graph.addVertex("y");
        graph.addEdge("x", "x");
        graph.addEdge("y", "x");
        CHECK(incident(graph, "x") == std::vector<edge_type>({{"x", "x"}, {"y", "x"}}));
        CHECK(incident(graph, "y") == std::vector<edge_type>({{"y", "x"}, {"y", "y"}}).empty()
                  ? std::vector<edge_type>({{"y", "x"}})
                  : std::vector<edge_type>({{"y", "x"}}));
    }
}

SUITE(GraphComparison) {
    TEST(EqualityComparesVerticesAndEdges) {
        const Graph graph = makeGraph();
        Graph same = makeGraph();
        CHECK(same == graph);
        CHECK(!(same != graph));
        same.addEdge("b", "c");
        CHECK(same != graph);
        CHECK(same > graph);
        CHECK(same >= graph);
        CHECK(!(same < graph));
        CHECK(graph < same);
        CHECK(graph <= same);
        CHECK(!(graph > same));
    }

    TEST(EqualityComparesVertexOrder) {
        Graph first;
        first.addVertex("a");
        first.addVertex("b");
        Graph second;
        second.addVertex("b");
        second.addVertex("a");
        CHECK(first != second);
        CHECK(first < second);
        CHECK(second > first);
    }

    TEST(SelfComparison) {
        const Graph graph = makeGraph();
        CHECK(graph == graph);
        CHECK(!(graph != graph));
        CHECK(!(graph < graph));
        CHECK(graph <= graph);
        CHECK(graph >= graph);
    }

    TEST(EmptyGraphComparison) {
        const Graph first;
        const Graph second;
        CHECK(first == second);
        CHECK(first <= second);
        CHECK(first >= second);
        Graph bigger;
        bigger.addVertex("a");
        CHECK(first < bigger);
        CHECK(bigger > first);
    }
}

SUITE(GraphOutput) {
    TEST(OutputStreamUsesIterators) {
        const Graph graph = makeGraph();
        std::ostringstream stream;
        stream << graph;
        CHECK(stream.str() == "{a -> [b c], b -> [d], c -> [a], d -> [a]}");
    }

    TEST(OutputStreamForEmptyGraph) {
        const Graph graph;
        std::ostringstream stream;
        stream << graph;
        CHECK(stream.str() == "{}");
    }

    TEST(OutputStreamForGraphWithoutEdges) {
        Graph graph;
        graph.addVertex("a");
        graph.addVertex("b");
        std::ostringstream stream;
        stream << graph;
        CHECK(stream.str() == "{a -> [], b -> []}");
    }

    TEST(OutputStreamAfterRemoval) {
        Graph graph = makeGraph();
        graph.removeVertex("b");
        std::ostringstream stream;
        stream << graph;
        CHECK(stream.str() == "{a -> [c], c -> [a], d -> [a]}");
    }
}

SUITE(GraphExceptions) {
    TEST(ExceptionsShareCommonBase) {
        Graph graph;
        bool caughtBase = false;
        try {
            graph.removeVertex("missing");
        } catch (const GraphException &) {
            caughtBase = true;
        }
        CHECK(caughtBase);

        caughtBase = false;
        try {
            graph.at(5);
        } catch (const GraphException &) {
            caughtBase = true;
        }
        CHECK(caughtBase);

        caughtBase = false;
        try {
            graph.edgeDegree("a", "b");
        } catch (const GraphException &) {
            caughtBase = true;
        }
        CHECK(caughtBase);

        caughtBase = false;
        try {
            graph.addVertex("x");
            graph.addVertex("x");
        } catch (const GraphException &) {
            caughtBase = true;
        }
        CHECK(caughtBase);

        caughtBase = false;
        try {
            graph.addVertex("x");
            graph.addVertex("y");
            graph.addEdge("x", "y");
            graph.addEdge("x", "y");
        } catch (const GraphException &) {
            caughtBase = true;
        }
        CHECK(caughtBase);
    }

    TEST(ExceptionMessagesAreProvided) {
        CHECK(std::string(NoSuchVertexException().what()).size() > 0u);
        CHECK(std::string(VertexAlreadyExistsException().what()).size() > 0u);
        CHECK(std::string(IndexOutOfRangeException().what()).size() > 0u);
        CHECK(std::string(EdgeAlreadyExistsException().what()).size() > 0u);
        CHECK(std::string(NoSuchEdgeException().what()).size() > 0u);
    }
}
