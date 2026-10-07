#include "cocktail_sort/cocktail_sort.hpp"
#include "oriented_graph/oriented_graph.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

static_assert(std::is_same_v<CocktailSorter<int *>::value_type, int>);
static_assert(std::is_same_v<CocktailSorter<std::vector<int>::iterator>::value_type, int>);
static_assert(std::is_same_v<OrientedGraph<std::string>::value_type, std::string>);
static_assert(std::is_same_v<OrientedGraph<std::string>::edge_type, int>);
static_assert(std::is_same_v<OrientedGraph<std::string>::container_type,
                             std::vector<GraphVertex<std::string>>>);
static_assert(std::is_same_v<GraphVertex<std::string>::edges_type, std::vector<int>>);
static_assert(std::is_same_v<OrientedGraph<std::string>::iterator,
                             OrientedGraph<std::string>::const_iterator>);
static_assert(std::is_same_v<OrientedGraph<std::string>::const_iterator::value_type, std::string>);

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

    OrientedGraph<std::string> graph;
    graph.addVertex("a");
    graph.addVertex("b");
    graph.addVertex("c");
    graph.addEdge("a", "b");
    graph.addEdge("a", "c");
    graph.addEdge("c", "a");

    const OrientedGraph<std::string>::edges_type fromA = graph.getEdges("a");
    const OrientedGraph<std::string>::edges_type fromC = graph.getEdges("c");

    std::cout << "edges(a):";
    for (OrientedGraph<std::string>::edge_type edge : fromA) {
        std::cout << ' ' << edge;
    }
    std::cout << '\n';

    const bool graph_ok = graph.size() == 3 && !graph.empty() && graph.contains("a") &&
                          !graph.contains("d") && fromA.size() == 2 && fromA[0] == 1 &&
                          fromA[1] == 2 && fromC.size() == 1 && fromC[0] == 0;

    bool graph_add_error_ok = false;
    try {
        graph.addEdge("a", "missing");
    } catch (const NoSuchVertexException &) {
        graph_add_error_ok = true;
    }

    bool graph_get_error_ok = false;
    try {
        graph.getEdges("missing");
    } catch (const NoSuchVertexException &) {
        graph_get_error_ok = true;
    }

    OrientedGraph<std::string> removalGraph;
    removalGraph.addVertex("a");
    removalGraph.addVertex("b");
    removalGraph.addVertex("c");
    removalGraph.addVertex("d");
    removalGraph.addEdge("a", "b");
    removalGraph.addEdge("a", "c");
    removalGraph.addEdge("c", "a");
    removalGraph.addEdge("d", "a");
    removalGraph.addEdge("b", "d");

    removalGraph.removeVertex("c");

    const OrientedGraph<std::string>::edges_type afterRemoveA = removalGraph.getEdges("a");
    const OrientedGraph<std::string>::edges_type afterRemoveB = removalGraph.getEdges("b");
    const OrientedGraph<std::string>::edges_type afterRemoveD = removalGraph.getEdges("d");

    std::cout << "after remove c -> a:";
    for (OrientedGraph<std::string>::edge_type edge : afterRemoveA) {
        std::cout << ' ' << edge;
    }
    std::cout << '\n';

    const bool graph_remove_vertex_ok = removalGraph.size() == 3 && !removalGraph.contains("c") &&
                                        afterRemoveA.size() == 1 && afterRemoveA[0] == 1 &&
                                        afterRemoveB.size() == 1 && afterRemoveB[0] == 2 &&
                                        afterRemoveD.size() == 1 && afterRemoveD[0] == 0;

    bool graph_remove_vertex_error_ok = false;
    try {
        removalGraph.removeVertex("c");
    } catch (const NoSuchVertexException &) {
        graph_remove_vertex_error_ok = true;
    }

    removalGraph.removeEdge("a", "b");
    const bool graph_remove_edge_ok = removalGraph.getEdges("a").empty();

    bool graph_remove_edge_error_ok = false;
    try {
        removalGraph.removeEdge("a", "b");
    } catch (const NoSuchEdgeException &) {
        graph_remove_edge_error_ok = true;
    }

    OrientedGraph<std::string> graphCopy = graph;
    OrientedGraph<std::string> graphDifferent;
    graphDifferent.addVertex("a");
    graphDifferent.addVertex("b");
    graphDifferent.addEdge("a", "b");

    const bool graph_compare_ok =
        graphCopy == graph && !(graphCopy != graph) && graph != graphDifferent &&
        graphCopy <= graph && graphCopy >= graph && !(graphCopy < graph) && !(graphCopy > graph) &&
        graphDifferent < graph && graph > graphDifferent;

    bool graph_duplicate_vertex_ok = false;
    try {
        graph.addVertex("a");
    } catch (const VertexAlreadyExistsException &) {
        graph_duplicate_vertex_ok = true;
    }

    bool graph_duplicate_edge_ok = false;
    try {
        graph.addEdge("a", "b");
    } catch (const EdgeAlreadyExistsException &) {
        graph_duplicate_edge_ok = true;
    }

    const bool graph_at_ok = graph.at(0) == "a" && graph.at(1) == "b" && graph.at(2) == "c";

    bool graph_at_error_ok = false;
    try {
        graph.at(graph.size());
    } catch (const IndexOutOfRangeException &) {
        graph_at_error_ok = true;
    }

    std::string iteratedValues;
    std::for_each(graph.cbegin(), graph.cend(),
                  [&](const std::string &value) { iteratedValues += value; });
    std::string manualValues;
    for (auto it = graph.cbegin(); it != graph.cend(); ++it)
        manualValues += *it;
    const bool graph_iterator_ok = iteratedValues == "abc" && manualValues == "abc" &&
                                   *(graph.cbegin()++) == "a" && graph.cbegin() != graph.cend() &&
                                   graph.cend() == graph.cend();

    OrientedGraph<std::string> graphAssigned;
    graphAssigned = graph;
    const bool graph_assign_ok = graphAssigned == graph && !(graphAssigned == graphDifferent);

    std::ostringstream graphOutput;
    graphOutput << graph;
    const bool graph_output_ok = graphOutput.str() == "{a -> [b c], b -> [], c -> [a]}";
    std::cout << "graph: " << graphOutput.str() << '\n';

    OrientedGraph<int> clearedGraph;
    clearedGraph.addVertex(1);
    clearedGraph.clear();
    const bool graph_clear_ok = clearedGraph.empty() && clearedGraph.size() == 0;

    const bool ok = array_ok && shuffled_ok && reversed_ok && single_ok && empty_ok && graph_ok &&
                    graph_add_error_ok && graph_get_error_ok && graph_remove_vertex_ok &&
                    graph_remove_vertex_error_ok && graph_remove_edge_ok &&
                    graph_remove_edge_error_ok && graph_compare_ok && graph_duplicate_vertex_ok &&
                    graph_duplicate_edge_ok && graph_at_ok && graph_at_error_ok &&
                    graph_iterator_ok && graph_assign_ok && graph_output_ok && graph_clear_ok;
    std::cout << (ok ? "all tests passed" : "tests failed") << '\n';
    return ok ? 0 : 1;
}
