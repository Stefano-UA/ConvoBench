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

    IFS='_' read -ra pretty_kernel <<< "$kernel"
    pretty_kernel="${pretty_kernel[@]^}"

    # Write a markdown link to the specific section
    echo "- [${pretty_kernel}](#kernel-${kernel})" >> "$OUTPUT_FILE"
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

    echo "" >> "$OUTPUT_FILE"
    
    # Add an anchor div so the links work
    echo "<div id=\"kernel-${kernel}\"></div>" >> "$OUTPUT_FILE"

    # Open the details block
    echo "<details>" >> "$OUTPUT_FILE"
    
    # Add the summary
    echo "<summary><strong>Kernel: ${kernel}</strong></summary>" >> "$OUTPUT_FILE"
    
    # Empty line required for the markdown table to render inside HTML
    echo "" >> "$OUTPUT_FILE"

    # Start table for this folder
    echo "| Original | Processed (${kernel}) |" >> "$OUTPUT_FILE"
    echo "| :------: | :-------: |" >> "$OUTPUT_FILE"

    # Iterate through images INSIDE this kernel folder
    for image in "$kernel_dir"/*; do

        # Check if it is actually a file
        [ -f "$image" ] || continue

        filename="$(basename "$image")"
        orig_path="${INPUT_DIR}/${filename%.*}"

        shopt -s nullglob
        # Get an array of matches
        matches=( "${orig_path}".{jpg,jpeg,png} )
        # Check if the array has at least one item
        [ ${#matches[@]} -gt 0 ] || continue
        orig_path="${matches[0]}"

        # Markdown requires RELATIVE paths to display images on GitHub.
        rel_path_input="$(realpath --relative-to="${HERE}/../" "$orig_path")"
        rel_path_output="$(realpath --relative-to="${HERE}/../" "$image")"

        # Append table row
        echo "| <img src=\"./${rel_path_input}\" width=\"100%\" /> | <img src=\"./${rel_path_output}\" width=\"100%\" /> |" >> "$OUTPUT_FILE"
    done

    # 5. Close the details block
    echo "</details>" >> "$OUTPUT_FILE"

done

# 4. Write Footer
cat >> "$OUTPUT_FILE" <<EOF

---
*Readme generated on $(date)*
EOF

echo "Generated ${OUTPUT_FILE} successfully."