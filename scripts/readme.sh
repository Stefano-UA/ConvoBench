#!/bin/bash

# Script directory
HERE="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

# Configuration
INPUT_DIR="${HERE}/../input"
OUTPUT_DIR="${HERE}/../output"
OUTPUT_FILE="${HERE}/../README.md"

# Check if directories exist
if [ ! -d "$INPUT_DIR" ] || [ ! -d "$OUTPUT_DIR" ]; then
    echo "Error: Directory '${INPUT_DIR}' or '${OUTPUT_DIR}' not found."
    exit 1
fi

# 1. Write Main Header
cat > "$OUTPUT_FILE" <<EOF
# Image Convolution Benchmark

Implementation of a convolution algorithm in C++ with the intention of using it as a simple means to benchmark CPU performance.

EOF

# 2. Write Index (Table of Contents)
echo "### Index" >> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

for kernel_dir in "$OUTPUT_DIR"/*; do
    # Check if it is actually a directory
    [ -d "$kernel_dir" ] || continue

    # Get the kernel name
    kernel="$(basename "$kernel_dir")"

    IFS='_' read -ra kernel <<< "$kernel"
    kernel="${kernel[@]^}"

    # Write a markdown link to the specific section
    # Note: GitHub Markdown converts "## Kernel: Name" to anchor "#kernel-name"
    echo "- [$kernel](#kernel-$kernel)" >> "$OUTPUT_FILE"
done

# Add some spacing after the index
echo -n "" >> "$OUTPUT_FILE"

# 3. Iterate through subdirectories in Output (Generate Tables)
for kernel_dir in "$OUTPUT_DIR"/*; do

    # Check if it is actually a directory
    [ -d "$kernel_dir" ] || continue

    # Get the kernel name
    kernel="$(basename "$kernel_dir")"

    echo "Processing section: $kernel..."

    # Write title for this kernel
    echo "" >> "$OUTPUT_FILE"
    echo "## Kernel: $kernel" >> "$OUTPUT_FILE"
    echo "" >> "$OUTPUT_FILE"

    # Start table for this folder
    echo "| Original | Processed ($kernel) |" >> "$OUTPUT_FILE"
    echo "| :------: | :-------: |" >> "$OUTPUT_FILE"

    # Iterate through images INSIDE this kernel folder
    for image in "$kernel_dir"/*; do

        # Check if it is actually a file
        [ -f "$image" ] || continue

        filename="$(basename "$image")"
        orig_path="${INPUT_DIR}/${filename}"

        # Check if the corresponding ORIGINAL file exists
        [ -f "$orig_path" ] || continue

        # Markdown requires RELATIVE paths to display images on GitHub.
        rel_path_input="./input/${filename}"
        rel_path_output="./output/${kernel}/${filename}"

        # Append table row
        echo "| <img src=\"${rel_path_input}\" width=\"300\" /> | <img src=\"${rel_path_output}\" width=\"300\" /> |" >> "$OUTPUT_FILE"
    done

done

# 4. Write Footer
cat >> "$OUTPUT_FILE" <<EOF

---
*Readme generated on $(date)*
EOF

echo "Generated ${OUTPUT_FILE} successfully."