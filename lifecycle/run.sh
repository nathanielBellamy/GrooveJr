#!/usr/bin/env bash
# lifecycle/run.sh — Run the built GrooveJr binary
#
# Locates the binary produced by lifecycle/build.sh and launches it.
#
# Usage:
#   ./lifecycle/run.sh                     # interactive (prompts for build dir)
#   ./lifecycle/run.sh -y                  # accept defaults
#   ./lifecycle/run.sh --build-dir <path>  # specify build directory explicitly

# ─── Setup ───────────────────────────────────────────────────────────

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/_common.sh"

BUILD_DIR_OVERRIDE=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        -y|--yes)
            GJ_AUTO_YES=1
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
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -y, --yes              Accept all defaults without prompting"
            echo "  --build-dir <path>     Path to the CMake build directory"
            echo "  -h, --help             Show this help"
            exit 0
            ;;
        *)
            warn "Unknown option: $1"
            shift
            ;;
    esac
done

trap 'echo ""; error "Run interrupted."; exit 130' INT

# ─── Main ─────────────────────────────────────────────────────────────

main() {
    banner "Run"

    detect_platform

    if [[ "$GJ_OS" == "windows" ]]; then
        warn "Native Windows is not yet fully supported."
        warn "Please use WSL (Windows Subsystem for Linux) and re-run this script."
        exit 1
    fi

    # ── Resolve build directory ──
    local build_dir
    if [[ -n "$BUILD_DIR_OVERRIDE" ]]; then
        build_dir="$BUILD_DIR_OVERRIDE"
    else
        build_dir=$(prompt_input "Build directory" "$DEFAULT_BUILD_DIR")
    fi

    # ── Locate and launch binary ──
    if [[ "$GJ_OS" == "macos" ]]; then
        local app_bundle="${build_dir}/GrooveJr.app"
        if [[ ! -d "$app_bundle" ]]; then
            error "App bundle not found: ${app_bundle}"
            info  "Run ./lifecycle/build.sh first to compile GrooveJr."
            exit 1
        fi
        info "Launching ${BOLD}${app_bundle}${RESET}"
        echo ""
        open "$app_bundle"
    else
        local binary="${build_dir}/GrooveJr"
        if [[ ! -f "$binary" ]]; then
            error "Binary not found: ${binary}"
            info  "Run ./lifecycle/build.sh first to compile GrooveJr."
            exit 1
        fi
        if [[ ! -x "$binary" ]]; then
            error "Binary is not executable: ${binary}"
            exit 1
        fi
        info "Launching ${BOLD}${binary}${RESET}"
        echo ""
        exec "$binary"
    fi
}

main
