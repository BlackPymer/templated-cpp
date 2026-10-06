#!/usr/bin/env bash
set -e
cd "$(dirname "$0")/.."

FILES=$(find include src -name '*.hpp' -o -name '*.cpp')

echo "== clang-format =="
clang-format --dry-run -Werror $FILES

echo "== clang-tidy =="
clang-tidy --extra-arg=-std=c++17 --extra-arg=-Iinclude src/*.cpp src/*/*.cpp || true
