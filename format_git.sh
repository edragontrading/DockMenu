#!/bin/bash

# Format only git modified or staged C++ files
for file in $(git diff --name-only --diff-filter=d | grep -E '\.(cpp|h|hpp|cxx|cc|c)$'); do
    if [ -f "$file" ]; then
        clang-format -i "$file"
        echo "Formatted: $file"
    fi
done
