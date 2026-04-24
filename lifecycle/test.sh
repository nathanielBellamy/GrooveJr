#!/usr/bin/env bash
set -euo pipefail

# ─────────────────────────────────────────────────────────────────────
# lifecycle/test.sh — Build and run the GrooveJr test suite
# ─────────────────────────────────────────────────────────────────────
# Usage:
#   ./lifecycle/test.sh              # interactive
#   ./lifecycle/test.sh -y           # non-interactive (accept defaults)
#   ./lifecycle/test.sh --filter '*Math*'  # run only matching tests
#   ./lifecycle/test.sh --verbose    # verbose CTest output
#
# This script:
#   1. Configures CMake with -DGJ_BUILD_TESTS=ON
#   2. Builds the test targets
#   3. Runs all tests via CTest
# ─────────────────────────────────────────────────────────────────────

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lifecycle/_common.sh
source "${SCRIPT_DIR}/_common.sh"

# ── Parse arguments ──────────────────────────────────────────────────

GTEST_FILTER=""
VERBOSE=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -y|--yes)
            export GJ_AUTO_YES=1
            shift
            ;;
        --filter)
            GTEST_FILTER="$2"
            shift 2
            ;;
        --filter=*)
            GTEST_FILTER="${1#*=}"
            shift
            ;;
        --verbose|-v)
            VERBOSE="--verbose"
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -y, --yes            Non-interactive mode (accept defaults)"
            echo "  --filter PATTERN     GTest filter pattern (e.g. '*Math*')"
            echo "  --verbose, -v        Show verbose test output"
            echo "  -h, --help           Show this help"
            exit 0
            ;;
        *)
            warn "Unknown option: $1"
            shift
            ;;
    esac
done

# ── Configuration ────────────────────────────────────────────────────

BUILD_DIR="${REPO_ROOT}/build-test"
BUILD_TYPE="Debug"

header "GrooveJr Test Runner"
info "Build directory: ${BUILD_DIR}"
info "Build type:      ${BUILD_TYPE}"
echo ""

# ── Check dependencies are installed ─────────────────────────────────

if ! command -v cmake &>/dev/null; then
    error "cmake is not installed. Run ./lifecycle/install.sh first."
    exit 1
fi

# ── CMake configure ──────────────────────────────────────────────────

if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" ]] || ! grep -q "GJ_BUILD_TESTS:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
    header "Configuring CMake (tests enabled)"

    CMAKE_ARGS=(
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
        -DGJ_BUILD_TESTS=ON
        -DGJ_ENABLE_ASAN=OFF
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    )

    # Platform-specific CMake args
    detect_platform
    case "${GJ_OS}" in
        macos)
            if command -v brew &>/dev/null; then
                CMAKE_ARGS+=(
                    "-DCMAKE_PREFIX_PATH=$(brew --prefix qt)"
                    "-DCAF_ROOT=$(brew --prefix caf)"
                )
            fi
            ;;
        linux|wsl)
            if [[ -d "${DEPS_DIR}/actor-framework/build" ]]; then
                CMAKE_ARGS+=("-DCAF_ROOT=${DEPS_DIR}/actor-framework/build")
            else
                CMAKE_ARGS+=("-DCAF_ROOT=/usr")
            fi
            ;;
    esac

    info "CMake args: ${CMAKE_ARGS[*]}"
    cmake -S "${SRC_DIR}" -B "${BUILD_DIR}" "${CMAKE_ARGS[@]}"
    success "CMake configured"
else
    info "CMake already configured with tests enabled"
fi

# ── Build test targets ───────────────────────────────────────────────

header "Building test targets"

JOBS=$(cpu_count)
info "Building with ${JOBS} parallel jobs"

cmake --build "${BUILD_DIR}" --target gj_test_pure gj_test_db -j "${JOBS}" 2>&1 | \
    while IFS= read -r line; do
        # Show progress but filter noise
        if [[ "${line}" == *"error:"* ]] || [[ "${line}" == *"Error"* ]]; then
            error "${line}"
        elif [[ "${line}" == *"warning:"* ]]; then
            warn "${line}"
        elif [[ "${line}" == *"Built target"* ]] || [[ "${line}" == *"Linking"* ]]; then
            info "${line}"
        fi
    done

if [[ ${PIPESTATUS[0]} -ne 0 ]]; then
    error "Build failed. See output above."
    exit 1
fi
success "Test targets built"

# ── Run tests ────────────────────────────────────────────────────────

header "Running tests"

CTEST_ARGS=(
    --test-dir "${BUILD_DIR}"
    --output-on-failure
)

if [[ -n "${VERBOSE}" ]]; then
    CTEST_ARGS+=("${VERBOSE}")
fi

# Pass GTest filter via environment
if [[ -n "${GTEST_FILTER}" ]]; then
    export GTEST_FILTER
    info "Filter: ${GTEST_FILTER}"
fi

echo ""
ctest "${CTEST_ARGS[@]}"
TEST_EXIT=$?

echo ""
if [[ ${TEST_EXIT} -eq 0 ]]; then
    success "All tests passed!"
else
    error "Some tests failed (exit code: ${TEST_EXIT})"
    echo ""
    info "Re-run with --verbose for detailed output"
    info "Or --filter 'TestName' to run specific tests"
fi

exit ${TEST_EXIT}
