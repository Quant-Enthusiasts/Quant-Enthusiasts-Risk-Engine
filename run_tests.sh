#!/bin/bash

# Script to run all test executables from the install directory
# This script assumes the project has been built and installed to cpp_engine/install/

set -e  # Exit on first error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_BIN_DIR="${SCRIPT_DIR}/cpp_engine/install/bin"

echo "============================================================"
echo "Running Quant Risk Engine Tests"
echo "============================================================"
echo ""

# Check if install directory exists
if [ ! -d "$INSTALL_BIN_DIR" ]; then
    echo "ERROR: Install directory not found: $INSTALL_BIN_DIR"
    echo ""
    echo "Please build and install the project first:"
    echo "  cd cpp_engine/build"
    echo "  cmake --build ."
    echo "  cmake --install ."
    exit 1
fi

# Find all test executables (files starting with "test_")
TEST_FILES=($(find "$INSTALL_BIN_DIR" -type f -name "test_*" | sort))

if [ ${#TEST_FILES[@]} -eq 0 ]; then
    echo "WARNING: No test executables found in $INSTALL_BIN_DIR"
    echo "Looking for files matching pattern: test_*"
    exit 0
fi

echo "Found ${#TEST_FILES[@]} test executable(s):"
for test_file in "${TEST_FILES[@]}"; do
    echo "  - $(basename "$test_file")"
done
echo ""

PASSED=0
FAILED=0
FAILED_TESTS=()

# Run each test
for test_file in "${TEST_FILES[@]}"; do
    test_name=$(basename "$test_file")
    echo "------------------------------------------------------------"
    echo "Running: $test_name"
    echo "------------------------------------------------------------"

    if "$test_file"; then
        echo "✓ $test_name PASSED"
        ((PASSED++))
    else
        echo "✗ $test_name FAILED"
        ((FAILED++))
        FAILED_TESTS+=("$test_name")
    fi
    echo ""
done

# Summary
echo "============================================================"
echo "Test Summary"
echo "============================================================"
echo "Total tests run: $((PASSED + FAILED))"
echo "Passed: $PASSED"
echo "Failed: $FAILED"

if [ $FAILED -gt 0 ]; then
    echo ""
    echo "Failed tests:"
    for failed_test in "${FAILED_TESTS[@]}"; do
        echo "  - $failed_test"
    done
    exit 1
else
    echo ""
    echo "All tests passed! ✓"
    exit 0
fi
