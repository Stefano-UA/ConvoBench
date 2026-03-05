#!/bin/bash
# <===============================================================================>

#   ██████╗ ███████╗███╗   ██╗ ██████╗██╗  ██╗███╗   ███╗ █████╗ ██████╗ ██╗  ██╗
#   ██╔══██╗██╔════╝████╗  ██║██╔════╝██║  ██║████╗ ████║██╔══██╗██╔══██╗██║ ██╔╝
#   ██████╔╝█████╗  ██╔██╗ ██║██║     ███████║██╔████╔██║███████║██████╔╝█████╔╝
#   ██╔══██╗██╔══╝  ██║╚██╗██║██║     ██╔══██║██║╚██╔╝██║██╔══██║██╔══██╗██╔═██╗
#   ██████╔╝███████╗██║ ╚████║╚██████╗██║  ██║██║ ╚═╝ ██║██║  ██║██║  ██║██║  ██╗
#   ╚═════╝ ╚══════╝╚═╝  ╚═══╝ ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝

# <===============================================================================>
#                          Validation Execution Script
# <===============================================================================>
#  Script directory
# <===============================================================================>
WKDIR="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"
# <===============================================================================>
#  Configuration
# <===============================================================================>
IN_DIR="input"
OUT_DIR="output"
BIN_DIR="${WKDIR}/../build"
COMP_DIR="${WKDIR}/../assets/test"
RUN_DIR="/tmp/convotest" # We run on RAM!
EXPECTED_FILES=18 # Expected total images generated per binary run
# <===============================================================================>
#  Build binaries if needed
# <===============================================================================>
pushd "${WKDIR}/.." &>/dev/null
echo -e "\nMaking binaries... (Be patient)\n"
# Only make O3 binaries
O_AXIS=O3 make -j$(nproc) >/dev/null
popd &>/dev/null
# <===============================================================================>
#  Record script start time
# <===============================================================================>
SCRIPT_START="$(date +%s)"
echo "<===============================================================================================>"
echo "                                   BINARIES VALIDATION REPORT                                    "
echo "<===============================================================================================>"
printf " %-70s | %-10s\n" "Binary Profile" "Status"
echo "<----------------------------------------------------------------------------------------------->"
# <===============================================================================>
#  Create run directory
# <===============================================================================>
mkdir -p "${RUN_DIR}/bin"
trap 'rm -fr "$RUN_DIR" &>/dev/null' EXIT
# <===============================================================================>
#  Copy required data to run directory
# <===============================================================================>
cp "${BIN_DIR}/main.O3".* "${RUN_DIR}/bin" 2>/dev/null || true
# We use input/ as our source image folder and compare_output/ for validation baseline
cp -r "${COMP_DIR}/${IN_DIR}" "${RUN_DIR}/${IN_DIR}"
# <===============================================================================>
#  Iterate over all valid executables
# <===============================================================================>
FAILED_COUNT=0
TOTAL_COUNT=0
for FILE_PATH in "${RUN_DIR}/bin/main".*; do
    # <===============================================================================>
    #  Skip if not a regular file or not executable
    # <===============================================================================>
    ([ ! -f "$FILE_PATH" ] || [ ! -x "$FILE_PATH" ]) && continue
    TOTAL_COUNT=$((TOTAL_COUNT + 1))
    basename="$(basename "$FILE_PATH")"
    # <===============================================================================>
    #  Run the binary in the test environment
    # <===============================================================================>
    pushd "$RUN_DIR" &>/dev/null
    # Remove output dir so environment is clean
    rm -fr "${RUN_DIR}/${OUT_DIR}" &> /dev/null
    # Run binary and capture exit code
    "$FILE_PATH" &>/dev/null
    EXIT_CODE=$?
    # <===============================================================================>
    #  Validate output count and byte-for-byte against compare output
    # <===============================================================================>
    MATCH_FAILED=0
    # Check total files generated
    ACTUAL_FILES="$(find "${RUN_DIR}/${OUT_DIR}" -type f 2>/dev/null | wc -l)"
    if [ "$ACTUAL_FILES" -ne "$EXPECTED_FILES" ]; then
        MATCH_FAILED=1
    fi
    if [ $EXIT_CODE -eq 0 ] && [ $MATCH_FAILED -eq 0 ]; then
        # Check all generated files against the baseline versions with matching names
        while IFS= read -r TEST_FILE; do
            # Compute relative path of the file
            REL_PATH="${TEST_FILE#${RUN_DIR}/${OUT_DIR}/}"
            REF_FILE="${COMP_DIR}/${OUT_DIR}/${REL_PATH}"
            #echo "$REF_FILE" "$TEST_FILE"
            if [ ! -f "$REF_FILE" ]; then
                MATCH_FAILED=1
                break
            fi
            # Compute Absolute Error (count of pixels exceeding a 2% variance threshold).
            RAW_OUT="$(compare -quiet -metric AE -fuzz 2% "$REF_FILE" "$TEST_FILE" null: 2>&1)"
            DIFF_PIXELS="$(echo "$RAW_OUT" | grep -oE '[0-9]+' | tail -n 1)"
            # Fail if empty (regex failed) or if any pixels exceeded the fuzz threshold
            if [ -z "$DIFF_PIXELS" ] || [ "$DIFF_PIXELS" -ne 0 ]; then
                MATCH_FAILED=1
                break
            fi
        done < <(find "${RUN_DIR}/${OUT_DIR}" -type f)
    fi
    popd &>/dev/null
    # <===============================================================================>
    #  Output to console
    # <===============================================================================>
    if [ $EXIT_CODE -eq 0 ] && [ $MATCH_FAILED -eq 0 ]; then
        printf " %-70s | \033[32m%-10s\033[0m\n" "$basename" "[PASS]"
    else
        printf " %-70s | \033[31m%-10s\033[0m\n" "$basename" "[FAIL]"
        FAILED_COUNT=$((FAILED_COUNT + 1))
    fi
done
# <===============================================================================>
#  Print a summary
# <===============================================================================>
echo "<===============================================================================================>"
echo " Validation completed. Passed: $((TOTAL_COUNT - FAILED_COUNT)) / ${TOTAL_COUNT}. Failed: ${FAILED_COUNT}."
echo "<===============================================================================================>"
exit $FAILED_COUNT