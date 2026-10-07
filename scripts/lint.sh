#!/usr/bin/env bash
set -e
cd "$(dirname "$0")/.."

FILES=$(find include src tests -name '*.hpp' -o -name '*.cpp')

echo "== clang-format =="
clang-format --dry-run -Werror $FILES

PINNED=$(grep -oE '[0-9]+\.[0-9]+\.[0-9]+' .clang-format-version 2>/dev/null | head -1 || true)
ACTUAL=$(clang-format --version | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
if [ -n "$PINNED" ] && [ "$PINNED" != "$ACTUAL" ]; then
    echo "warning: local clang-format $ACTUAL differs from pinned $PINNED (used by CI)" >&2
    echo "         install it with: pip install clang-format==$PINNED" >&2
fi

echo "== clang-tidy =="
if ! command -v clang-tidy >/dev/null 2>&1; then
    echo "skipped: clang-tidy not installed"
elif [ ! -f compile_commands.json ]; then
    echo "skipped: compile_commands.json not found (run cmake first)"
else
    clang-tidy --extra-arg=-std=c++17 --extra-arg=-Iinclude \
        src/*.cpp src/*/*.cpp tests/*.cpp || true
fi
