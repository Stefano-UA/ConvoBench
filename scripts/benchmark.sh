#!/bin/bash

# Script directory
HERE="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

# Configuration
BIN_DIR="${HERE}/../build"
BINARIES=("main.o2" "main.o3")

echo "========================================"
echo "      PERFORMANCE BENCHMARK REPORT      "
echo "========================================"
printf "%-15s | %-15s\n" "Binary" "Time (seconds)"
echo "----------------------------------------"

for bin in "${BINARIES[@]}"; do
    FILE_PATH="${BIN_DIR}/${bin}"

    # Check if binary exists
    if [ ! -f "$FILE_PATH" ]; then
        printf "%-15s | %-15s\n" "${bin}" "Not Found"
        continue
    fi

    # Remove output dir so environment is constant
    rm -fr "${HERE}/../output"

    # Capture start time (nanoseconds)
    start_time="$(date +%s%N)"

    # Run the binary
    # We redirect stdout/stderr to /dev/null to lower the impact printing has on speeed
    "$FILE_PATH" > /dev/null 2>&1

    # Capture end time
    end_time="$(date +%s%N)"

    # Calculate duration in seconds (floating point math using bc)
    duration="$(echo "scale=4; ($end_time - $start_time) / 1000000000" | bc)"

    # Output result
    printf "%-15s | %s s\n" "$bin" "$duration"
done

echo "========================================"