#include "cocktail_sort/cocktail_sort.hpp"
#include "cocktail_sort/test_container.hpp"
#include "cocktail_sort/test_type.hpp"
#include "oriented_graph/oriented_graph.hpp"
#include "strand_sort/strand_sort.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace {

using Graph = OrientedGraph<std::string>;
using edge_type = Graph::edge_type;

template <typename Container> void print(const Container &values) {
    for (const auto &value : values)
        std::cout << ' ' << value;
    std::cout << '\n';
}

bool readLine(const std::string &prompt, std::string &line) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, line));
}

bool readChoice(int &choice) {
    std::string line;
    if (!readLine("> ", line))
        return false;
    std::istringstream input(line);
    input >> choice;
    if (!input || !(choice >= 0 && choice <= 20)) {
        choice = -1;
        return true;
    }
    return true;
}

std::vector<int> randomValues(std::size_t count) {
    std::vector<int> values(count);
    for (std::size_t i = 0; i < count; ++i)
        values[i] = static_cast<int>(i);
    std::shuffle(values.begin(), values.end(), std::mt19937(42));
    return values;
}

std::vector<int> readValues() {
    std::string line;
    if (!readLine("values (integers separated by spaces): ", line))
        return {};
    std::vector<int> values;
    std::istringstream input(line);
    int value = 0;
    while (input >> value)
        values.push_back(value);
    return values;
}

void showMenu() {
    std::cout << "\n-- sorting --\n"
              << " 1) Cocktail sort, vector<int>          2) Strand sort, vector<int>\n"
              << " 3) Cocktail sort, custom objects       4) Strand sort, custom objects\n"
              << " 5) Cocktail sort, custom container     6) Compare with std::sort\n"
              << " 7) Sort your own values\n"
              << "-- graph --\n"
              << " 8) Build demo graph                    9) Add vertex\n"
              << "10) Remove vertex                      11) Add edge\n"
              << "12) Remove edge                        13) Queries\n"
              << "14) Iterators                          15) Exception demo\n"
              << "16) Copy, compare, output              17) Interactive edit\n"
              << " 0) Exit\n"
              << "[1-17] ";
}

void demoCocktailVector() {
    const std::vector<int> values = randomValues(16);
    std::vector<int> work = values;
    CocktailSorter<std::vector<int>::iterator> sorter;
    sorter.Sort(work.begin(), work.end());
    const bool ok = std::is_sorted(work.begin(), work.end());
    std::cout << "before:";
    print(values);
    std::cout << "after: ";
    print(work);
    std::cout << (ok ? "sorted" : "FAILED") << '\n';
}

void demoStrandVector() {
    const std::vector<int> values = randomValues(16);
    std::vector<int> work = values;
    StrandSorter<std::vector<int>::iterator> sorter;
    sorter.Sort(work.begin(), work.end());
    const bool ok = std::is_sorted(work.begin(), work.end());
    std::cout << "before:";
    print(values);
    std::cout << "after: ";
    print(work);
    std::cout << (ok ? "sorted" : "FAILED") << '\n';
}

void demoCocktailStability() {
    TestType items[] = {TestType(5, 0), TestType(1, 1), TestType(3, 2), TestType(2, 3),
                        TestType(1, 4)};
    std::cout << "before:";
    for (const TestType &item : items)
        std::cout << ' ' << item;
    CocktailSorter<TestType *> sorter;
    sorter.Sort(items, items + 5);
    std::cout << "\nafter: ";
    for (const TestType &item : items)
        std::cout << ' ' << item;
    bool stable = true;
    for (int i = 1; i < 5; ++i)
        stable = stable && !(items[i - 1].value == items[i].value && items[i - 1].id > items[i].id);
    std::cout << (stable ? "stable" : "FAILED") << '\n';
}

void demoStrandObjects() {
    const std::vector<TestType> expected = {TestType(1, 1), TestType(2, 3), TestType(3, 2),
                                            TestType(5, 0)};
    std::vector<TestType> values = {TestType(5, 0), TestType(1, 1), TestType(3, 2), TestType(2, 3)};
    std::cout << "before:";
    for (const TestType &item : values)
        std::cout << ' ' << item;
    StrandSorter<std::vector<TestType>::iterator> sorter;
    sorter.Sort(values.begin(), values.end());
    std::cout << "\nafter: ";
    for (const TestType &item : values)
        std::cout << ' ' << item;
    std::cout << (values == expected ? "as expected" : "FAILED") << '\n';
}

void demoCustomContainer() {
    TestContainer values = {9, 4, 7, 1, 8, 3};
    std::cout << "before:";
    print(values);
    CocktailSorter<TestContainer::iterator> sorter;
    sorter.Sort(values.begin(), values.end());
    std::cout << "after: ";
    print(values);
    std::cout << (values == TestContainer({1, 3, 4, 7, 8, 9}) ? "sorted" : "FAILED") << '\n';
}

void compareWithStd() {
    const std::vector<int> values = randomValues(2000);
    std::vector<int> expected = values;
    std::sort(expected.begin(), expected.end());
    std::vector<int> expectedStable = values;
    std::stable_sort(expectedStable.begin(), expectedStable.end());

    std::vector<int> cocktail = values;
    std::vector<int> strand = values;
    const auto start = std::chrono::steady_clock::now();
    CocktailSorter<std::vector<int>::iterator> cocktailSorter;
    cocktailSorter.Sort(cocktail.begin(), cocktail.end());
    const auto middle = std::chrono::steady_clock::now();
    StrandSorter<std::vector<int>::iterator> strandSorter;
    strandSorter.Sort(strand.begin(), strand.end());
    const auto finish = std::chrono::steady_clock::now();

    const auto cocktailMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(middle - start).count();
    const auto strandMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(finish - middle).count();

    std::cout << "cocktail == std::sort:        " << (cocktail == expected ? "yes" : "no") << '\n'
              << "strand   == std::stable_sort: " << (strand == expectedStable ? "yes" : "no")
              << '\n'
              << "time: cocktail " << cocktailMs << " ms, strand " << strandMs << " ms\n";
}

void sortOwnValues() {
    std::vector<int> values = readValues();
    if (values.empty()) {
        std::cout << "nothing to sort\n";
        return;
    }
    std::vector<int> cocktail = values;
    std::vector<int> strand = values;
    CocktailSorter<std::vector<int>::iterator> cocktailSorter;
    cocktailSorter.Sort(cocktail.begin(), cocktail.end());
    StrandSorter<std::vector<int>::iterator> strandSorter;
    strandSorter.Sort(strand.begin(), strand.end());
    std::cout << "cocktail:";
    print(cocktail);
    std::cout << "strand:  ";
    print(strand);
    std::cout << (cocktail == strand ? "results match" : "results differ") << '\n';
}

Graph makeDemoGraph() {
    Graph graph;
    for (const char *vertex : {"a", "b", "c", "d"})
        graph.addVertex(vertex);
    graph.addEdge("a", "b");
    graph.addEdge("a", "c");
    graph.addEdge("c", "a");
    graph.addEdge("d", "a");
    graph.addEdge("b", "d");
    return graph;
}

std::string readVertex(const std::string &prompt) {
    std::string line;
    if (!readLine(prompt, line))
        return {};
    std::istringstream input(line);
    std::string vertex;
    input >> vertex;
    return vertex;
}

std::pair<std::string, std::string> readEdge() {
    const std::string from = readVertex("from: ");
    const std::string to = readVertex("to:   ");
    return {from, to};
}

template <typename Function> void report(Function action) {
    try {
        action();
    } catch (const GraphException &exception) {
        std::cout << "error: " << exception.what() << '\n';
    }
}

void demoGraph() {
    const Graph graph = makeDemoGraph();
    std::cout << graph << '\n';
    std::cout << "size: " << graph.size() << ", edges: " << graph.edgeCount()
              << ", empty: " << (graph.empty() ? "yes" : "no") << '\n';
    std::cout << "degree(a): " << graph.degree("a")
              << ", edgeDegree(a,b): " << graph.edgeDegree("a", "b")
              << ", hasEdge(c,a): " << (graph.hasEdge("c", "a") ? "yes" : "no") << '\n';
}

void graphAddVertex(Graph &graph) {
    const std::string vertex = readVertex("vertex: ");
    report([&] {
        graph.addVertex(vertex);
        std::cout << "added, size is now " << graph.size() << '\n';
    });
}

void graphRemoveVertex(Graph &graph) {
    const std::string vertex = readVertex("vertex: ");
    report([&] {
        graph.removeVertex(vertex);
        std::cout << "removed, size is now " << graph.size() << '\n';
    });
}

void graphAddEdge(Graph &graph) {
    const auto [from, to] = readEdge();
    if (from.empty() || to.empty()) {
        std::cout << "bad edge\n";
        return;
    }
    report([&] {
        graph.addEdge(from, to);
        std::cout << "added, edges: " << graph.edgeCount() << '\n';
    });
}

void graphRemoveEdge(Graph &graph) {
    const auto [from, to] = readEdge();
    if (from.empty() || to.empty()) {
        std::cout << "bad edge\n";
        return;
    }
    report([&] {
        graph.removeEdge(from, to);
        std::cout << "removed, edges: " << graph.edgeCount() << '\n';
    });
}

void graphQueries(const Graph &graph) {
    const std::string vertex = readVertex("vertex: ");
    report([&] {
        std::cout << "contains: " << (graph.contains(vertex) ? "yes" : "no")
                  << ", indexOf: " << graph.indexOf(vertex) << ", degree: " << graph.degree(vertex)
                  << '\n';
    });
    const std::string indexLine = readVertex("index: ");
    if (!indexLine.empty()) {
        std::istringstream input(indexLine);
        std::size_t index = 0;
        input >> index;
        if (input)
            report([&] { std::cout << "at(" << index << ") = " << graph.at(index) << '\n'; });
    }
    const auto [from, to] = readEdge();
    if (!from.empty() && !to.empty())
        report([&] {
            std::cout << "hasEdge: " << (graph.hasEdge(from, to) ? "yes" : "no")
                      << ", edgeDegree: " << graph.edgeDegree(from, to) << '\n';
        });
}

void graphIterators(const Graph &graph) {
    std::cout << "vertices forward:";
    for (auto it = graph.begin(); it != graph.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << "\nvertices backward:";
    auto it = graph.end();
    while (it != graph.begin()) {
        --it;
        std::cout << ' ' << *it;
    }
    std::cout << "\nvertices reverse:";
    for (auto rit = graph.rbegin(); rit != graph.rend(); ++rit)
        std::cout << ' ' << *rit;

    std::cout << "\nedges forward:";
    for (auto edge = graph.edgeBegin(); edge != graph.edgeEnd(); ++edge)
        std::cout << " (" << (*edge).first << "->" << (*edge).second << ')';
    std::cout << "\nedges reverse:";
    for (auto edge = graph.edgeRBegin(); edge != graph.edgeREnd(); ++edge)
        std::cout << " (" << (*edge).first << "->" << (*edge).second << ')';

    const std::string vertex = readVertex("\nvertex for neighbours: ");
    report([&] {
        std::cout << "neighbours:";
        for (auto neighbour = graph.neighborBegin(vertex); neighbour != graph.neighborEnd(vertex);
             ++neighbour)
            std::cout << ' ' << *neighbour;
        std::cout << "\nincident:";
        for (auto edge = graph.incidentBegin(vertex); edge != graph.incidentEnd(vertex); ++edge)
            std::cout << " (" << (*edge).first << "->" << (*edge).second << ')';
        std::cout << "\nincident reverse:";
        for (auto edge = graph.incidentRBegin(vertex); edge != graph.incidentREnd(vertex); ++edge)
            std::cout << " (" << (*edge).first << "->" << (*edge).second << ')';
        std::cout << '\n';
    });
}

void graphExceptions(Graph &graph) {
    report([&] {
        graph.addVertex(graph.empty() ? "a" : graph.at(0));
        std::cout << "duplicate vertex accepted (unexpected)\n";
    });
    report([&] {
        graph.removeVertex("no-such-vertex");
        std::cout << "missing vertex accepted (unexpected)\n";
    });
    report([&] {
        graph.at(graph.size());
        std::cout << "out of range accepted (unexpected)\n";
    });
    if (graph.size() > 1) {
        report([&] {
            graph.addEdge(graph.at(0), graph.at(0));
            graph.addEdge(graph.at(0), graph.at(0));
        });
    }
}

void graphCopyAndCompare(Graph &graph) {
    Graph copy = graph;
    std::cout << "copy:   " << copy << '\n';
    std::cout << "equal:  " << (copy == graph ? "yes" : "no")
              << ", less: " << (copy < graph ? "yes" : "no")
              << ", greater: " << (copy > graph ? "yes" : "no") << '\n';
    if (!graph.empty()) {
        Graph smaller = graph;
        smaller.removeVertex(smaller.at(0));
        std::cout << "smaller: " << smaller << '\n'
                  << "smaller < original: " << (smaller < graph ? "yes" : "no") << '\n';
    }
}

void graphInteractive(Graph &graph) {
    std::cout << "graph: " << graph << '\n';
    std::cout << "commands: addv <name> | delv <name> | adde <from> <to> | dele <from> <to> | "
                 "show | degree <name> | quit\n";
    std::string line;
    while (readLine("graph> ", line)) {
        std::istringstream input(line);
        std::string command;
        input >> command;
        if (command == "quit" || command.empty())
            return;
        if (command == "show") {
            std::cout << graph << '\n';
            continue;
        }
        std::string first;
        std::string second;
        input >> first >> second;
        report([&] {
            if (command == "addv" && !first.empty()) {
                graph.addVertex(first);
            } else if (command == "delv" && !first.empty()) {
                graph.removeVertex(first);
            } else if (command == "adde" && !first.empty() && !second.empty()) {
                graph.addEdge(first, second);
            } else if (command == "dele" && !first.empty() && !second.empty()) {
                graph.removeEdge(first, second);
            } else if (command == "degree" && !first.empty()) {
                std::cout << graph.degree(first) << '\n';
                return;
            } else {
                std::cout << "unknown command\n";
                return;
            }
            std::cout << graph << '\n';
        });
    }
}

} // namespace

int main() {
    Graph graph = makeDemoGraph();
    std::cout << "Lab 4: sorting algorithms and oriented graph container\n";
    bool running = true;
    while (running) {
        showMenu();
        int choice = -1;
        if (!readChoice(choice))
            return 0;
        switch (choice) {
        case 1:
            demoCocktailVector();
            break;
        case 2:
            demoStrandVector();
            break;
        case 3:
            demoCocktailStability();
            break;
        case 4:
            demoStrandObjects();
            break;
        case 5:
            demoCustomContainer();
            break;
        case 6:
            compareWithStd();
            break;
        case 7:
            sortOwnValues();
            break;
        case 8:
            demoGraph();
            break;
        case 9:
            graphAddVertex(graph);
            break;
        case 10:
            graphRemoveVertex(graph);
            break;
        case 11:
            graphAddEdge(graph);
            break;
        case 12:
            graphRemoveEdge(graph);
            break;
        case 13:
            graphQueries(graph);
            break;
        case 14:
            graphIterators(graph);
            break;
        case 15:
            graphExceptions(graph);
            break;
        case 16:
            graphCopyAndCompare(graph);
            break;
        case 17:
            graphInteractive(graph);
            break;
        case 0:
            running = false;
            break;
        default:
            std::cout << "unknown command\n";
            break;
        }
    }
    std::cout << "bye\n";
    return 0;
}
