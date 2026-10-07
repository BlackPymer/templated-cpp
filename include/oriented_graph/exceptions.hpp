#pragma once
#include <exception>

class GraphException : public std::exception {};
class NoSuchVertexException : public GraphException {
    public:
    const char *what() const noexcept override { return "oriented graph: no such vertex"; }
};
class VertexAlreadyExistsException : public GraphException {
    public:
    const char *what() const noexcept override { return "oriented graph: vertex already exists"; }
};
class IndexOutOfRangeException : public GraphException {
    public:
    const char *what() const noexcept override { return "oriented graph: index out of range"; }
};
class EdgeAlreadyExistsException : public GraphException {
    public:
    const char *what() const noexcept override { return "oriented graph: edge already exists"; }
};
class NoSuchEdgeException : public GraphException {
    public:
    const char *what() const noexcept override { return "oriented graph: no such edge"; }
};