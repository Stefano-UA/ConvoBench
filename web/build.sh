#!/bin/bash
# <=======================================================>

#   ██╗    ██╗███████╗██████╗  ██████╗ ███████╗███╗   ██╗
#   ██║    ██║██╔════╝██╔══██╗██╔════╝ ██╔════╝████╗  ██║
#   ██║ █╗ ██║█████╗  ██████╔╝██║  ███╗█████╗  ██╔██╗ ██║
#   ██║███╗██║██╔══╝  ██╔══██╗██║   ██║██╔══╝  ██║╚██╗██║
#   ╚███╔███╔╝███████╗██████╔╝╚██████╔╝███████╗██║ ╚████║
#    ╚══╝╚══╝ ╚══════╝╚═════╝  ╚═════╝ ╚══════╝╚═╝  ╚═══╝

# <=======================================================>
#      Generates static web page using HTML snippets
# <=======================================================>
#  Stop on error
# <=======================================================>
set -e
# <=======================================================>
#  Configuration & Constants:
# <=======================================================>
#  Script directory
# <=======================================================>
WKDIR="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"
# <=======================================================>
#  HTML Snippets & CSS
# <=======================================================>
HEADER_SNP="${WKDIR}/header.html"
FOOTER_SNP="${WKDIR}/footer.html"
SECTION_SNP="${WKDIR}/section.html"
SECTION_END_SNP="${WKDIR}/section_end.html"
CARD_SNP="${WKDIR}/card.html"
CSS_SRC="${WKDIR}/style.css"
# <=======================================================>
#  Input directories
# <=======================================================>
INPUT_DIR="${WKDIR}/../input"
OUTPUT_DIR="${WKDIR}/../output"
# <=======================================================>
#  Output directory
# <=======================================================>
DIST_DIR="${WKDIR}/dist"
# <=======================================================>
#  Code:
# <=======================================================>
#  Clean and create directories
# <=======================================================>
echo " -> Cleaning output directory: ${DIST_DIR}"
rm -fr "$DIST_DIR"
mkdir -p "$DIST_DIR"
# <=======================================================>
#  Check if outputs exist
# <=======================================================>
if [ ! -d "$OUTPUT_DIR" ]; then
    echo " [ERROR] No output directory found at ${OUTPUT_DIR}. Please make them first."
    exit 1
fi
# <=======================================================>
#  Copy assets to distribution folder
# <=======================================================>
echo " -> Copying images from ${INPUT_DIR} and ${OUTPUT_DIR}..."
ln -sfr "$INPUT_DIR" "${DIST_DIR}/input"
ln -sfr "$OUTPUT_DIR" "${DIST_DIR}/output"
# <=======================================================>
echo " -> Copying styles..."
if [ -f "$CSS_SRC" ]; then
    cp "$CSS_SRC" "${DIST_DIR}/"
else
    echo " [WARNING] Style file not found at ${CSS_SRC}. Generating without CSS."
fi
# <=======================================================>
#  Generate index.html
# <=======================================================>
INDEX_FILE="${DIST_DIR}/index.html"
echo " -> Building ${INDEX_FILE} from snippets..."
# <=======================================================>
#  1. Inject Header
# <=======================================================>
if [ -f "$HEADER_SNP" ]; then
    cat "$HEADER_SNP" > "$INDEX_FILE"
else
    echo " [ERROR] Header template not found at ${HEADER_SNP}"
    exit 1
fi
# <=======================================================>
#  2. Inject Sections and Cards (Loop & Replace)
# <=======================================================>
if [ -f "$SECTION_SNP" ] && [ -f "$CARD_SNP" ] && [ -f "$SECTION_END_SNP" ]; then
    for kernel_dir in "$OUTPUT_DIR"/*; do
        # Check if directory
        [ -d "$kernel_dir" ] || continue
        kernelname="$(basename "$kernel_dir")"
        # Format title: (replace underscores with spaces, capitalize)
        title="$(echo "$kernelname" | sed -e 's/_/ /g' | awk '{for(i=1;i<=NF;i++)sub(/./,toupper(substr($i,1,1)),$i)}1')"
        echo "    -> Processing section: $title"
        # Inject Section Start
        sed -e "s|{{KERNEL_ID}}|${kernelname}|g" -e "s|{{TITLE}}|${title}|g" "$SECTION_SNP" >> "$INDEX_FILE"
        # Iterate through images inside this kernel folder in the dist
        for image in "${DIST_DIR}/output/${kernelname}"/*; do
            [ -f "$image" ] || continue
            filename="$(basename "$image")"
            img_name="${filename%.*}"
            orig_path="${DIST_DIR}/input/${img_name}"
            shopt -s nullglob
            matches=( "${orig_path}".{jpg,jpeg,png,bmp} )
            shopt -u nullglob
            [ ${#matches[@]} -gt 0 ] || continue
            orig_file="$(basename "${matches[0]}")"
            # Format image title
            img_title="$(echo "$img_name" | sed -e 's/_/ /g' | awk '{for(i=1;i<=NF;i++)sub(/./,toupper(substr($i,1,1)),$i)}1')"
            # URLs relative to index.html
            orig_url="input/${orig_file}"
            proc_url="output/${kernelname}/${filename}"
            # Replace and append card
            sed -e "s|{{IMAGE_NAME}}|${img_title}|g" \
                -e "s|{{ORIG_URL}}|${orig_url}|g" \
                -e "s|{{PROC_URL}}|${proc_url}|g" \
                -e "s|{{KERNEL_NAME}}|${title}|g" \
                "$CARD_SNP" >> "$INDEX_FILE"
        done
        # Inject Section End
        cat "$SECTION_END_SNP" >> "$INDEX_FILE"
    done
else
    echo " [ERROR] One or more section/card templates not found."
    exit 1
fi
# <=======================================================>
#  3. Inject Footer
# <=======================================================>
if [ -f "$FOOTER_SNP" ]; then
    cat "$FOOTER_SNP" >> "$INDEX_FILE"
else
    echo " [ERROR] Footer template not found at ${FOOTER_SNP}"
    exit 1
fi
# <=======================================================>
#  End of Script
# <=======================================================>
echo " -> Success! Website generated at: ${DIST_DIR}/index.html"
# <=======================================================>