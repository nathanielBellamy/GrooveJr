#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lifecycle/_common.sh
source "${SCRIPT_DIR}/_common.sh"

DEFAULT_SQUISH_SUITE_DIR="${REPO_ROOT}/tests/squish/suite_groovejr"
BUILD_DIR_OVERRIDE=""
SUITE_DIR_OVERRIDE=""
SQUISH_RUNNER_OVERRIDE=""
TESTCASE=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -y|--yes)
            export GJ_AUTO_YES=1
            shift
            ;;
        --build-dir)
            BUILD_DIR_OVERRIDE="$2"
            shift 2
            ;;
        --build-dir=*)
            BUILD_DIR_OVERRIDE="${1#*=}"
            shift
            ;;
        --suite-dir)
            SUITE_DIR_OVERRIDE="$2"
            shift 2
            ;;
        --suite-dir=*)
            SUITE_DIR_OVERRIDE="${1#*=}"
            shift
            ;;
        --runner)
            SQUISH_RUNNER_OVERRIDE="$2"
            shift 2
            ;;
        --runner=*)
            SQUISH_RUNNER_OVERRIDE="${1#*=}"
            shift
            ;;
        --testcase)
            TESTCASE="$2"
            shift 2
            ;;
        --testcase=*)
            TESTCASE="${1#*=}"
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -y, --yes              Non-interactive mode (accept defaults)"
            echo "  --build-dir <path>     Path to the CMake build directory"
            echo "  --suite-dir <path>     Path to the Squish test suite"
            echo "  --runner <path>        Path to squishrunner"
            echo "  --testcase <name>      Run a single Squish testcase"
            echo "  -h, --help             Show this help"
            exit 0
            ;;
        *)
            warn "Unknown option: $1"
            shift
            ;;
    esac
done

find_squish_runner() {
    if [[ -n "${SQUISH_RUNNER_OVERRIDE}" ]]; then
        echo "${SQUISH_RUNNER_OVERRIDE}"
        return 0
    fi

    if [[ -n "${SQUISH_RUNNER:-}" ]]; then
        echo "${SQUISH_RUNNER}"
        return 0
    fi

    if [[ -n "${SQUISH_PREFIX:-}" ]] && [[ -x "${SQUISH_PREFIX}/bin/squishrunner" ]]; then
        echo "${SQUISH_PREFIX}/bin/squishrunner"
        return 0
    fi

    if command -v squishrunner &>/dev/null; then
        command -v squishrunner
        return 0
    fi

    error "Unable to find squishrunner."
    info "Set SQUISH_PREFIX, set SQUISH_RUNNER, or pass --runner <path>."
    exit 1
}

resolve_build_dir() {
    if [[ -n "${BUILD_DIR_OVERRIDE}" ]]; then
        echo "${BUILD_DIR_OVERRIDE}"
    else
        prompt_input "Build directory" "${DEFAULT_BUILD_DIR}"
    fi
}

resolve_suite_dir() {
    if [[ -n "${SUITE_DIR_OVERRIDE}" ]]; then
        echo "${SUITE_DIR_OVERRIDE}"
    else
        prompt_input "Squish suite directory" "${DEFAULT_SQUISH_SUITE_DIR}"
    fi
}

resolve_aut_path() {
    local build_dir="$1"

    case "${GJ_OS}" in
        macos)
            echo "${build_dir}/GrooveJr.app/Contents/MacOS/GrooveJr"
            ;;
        windows)
            echo "${build_dir}/GrooveJr.exe"
            ;;
        *)
            echo "${build_dir}/GrooveJr"
            ;;
    esac
}

main() {
    banner "Squish"
    detect_platform

    local build_dir suite_dir squish_runner aut_path
    build_dir="$(resolve_build_dir)"
    suite_dir="$(resolve_suite_dir)"
    squish_runner="$(find_squish_runner)"
    aut_path="$(resolve_aut_path "${build_dir}")"

    if [[ ! -d "${suite_dir}" ]]; then
        error "Squish suite directory not found: ${suite_dir}"
        exit 1
    fi

    if [[ ! -f "${suite_dir}/suite.conf" ]]; then
        error "Missing suite.conf in ${suite_dir}"
        exit 1
    fi

    if [[ ! -f "${aut_path}" ]]; then
        error "GrooveJr executable not found: ${aut_path}"
        info "Build the app first with ./lifecycle/build.sh"
        exit 1
    fi

    header "Squish Configuration"
    info "Suite:      ${suite_dir}"
    info "Runner:     ${squish_runner}"
    info "Build dir:  ${build_dir}"
    info "Executable: ${aut_path}"
    if [[ -n "${TESTCASE}" ]]; then
        info "Test case:  ${TESTCASE}"
    fi
    echo ""
    info "If Squish cannot connect to the AUT, start squishserver from your Squish install and retry."
    echo ""

    export GJ_SQUISH_BUILD_DIR="${build_dir}"
    export GJ_SQUISH_AUT="${aut_path}"

    local squish_args=(
        --testsuite "${suite_dir}"
    )

    if [[ -n "${TESTCASE}" ]]; then
        squish_args+=(
            --testcase "${TESTCASE}"
        )
    fi

    "${squish_runner}" "${squish_args[@]}"
}

main
