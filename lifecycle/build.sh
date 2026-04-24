#!/usr/bin/env bash
# lifecycle/build.sh — GrooveJr Interactive Build Script
#
# Checks dependencies, prompts for build configuration, then
# configures and compiles the project.
#
# Usage:
#   ./lifecycle/build.sh              # interactive
#   ./lifecycle/build.sh -y           # accept all defaults (Debug build, system compiler)

# ─── Setup ───────────────────────────────────────────────────────────

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/_common.sh"

for arg in "$@"; do
    case "$arg" in
        -y|--yes) GJ_AUTO_YES=1 ;;
        -h|--help)
            echo "Usage: $0 [-y|--yes] [-h|--help]"
            echo "  -y, --yes    Accept all defaults without prompting"
            exit 0
            ;;
    esac
done

trap 'echo ""; error "Build interrupted."; exit 130' INT

# ─── Dependency Check ────────────────────────────────────────────────

check_dependencies() {
    header "Checking Dependencies"
    local missing=0

    # cmake
    if command -v cmake &>/dev/null; then
        success "cmake $(cmake --version | head -1 | awk '{print $3}')"
    else
        error "cmake — not found"
        missing=$((missing + 1))
    fi

    # Qt6
    local qt_ok=false
    case "$GJ_OS" in
        macos)
            if brew list qt &>/dev/null 2>&1; then qt_ok=true; fi
            ;;
        linux|wsl)
            if pkg-config --exists Qt6Core 2>/dev/null; then
                qt_ok=true
            elif dpkg -s qt6-base-dev &>/dev/null 2>&1; then
                qt_ok=true
            elif rpm -q qt6-qtbase-devel &>/dev/null 2>&1; then
                qt_ok=true
            elif pacman -Q qt6-base &>/dev/null 2>&1; then
                qt_ok=true
            fi
            ;;
    esac
    if $qt_ok; then
        success "Qt6"
    else
        error "Qt6 — not found"
        missing=$((missing + 1))
    fi

    # Boost
    local boost_ok=false
    case "$GJ_OS" in
        macos)
            if brew list boost &>/dev/null 2>&1; then boost_ok=true; fi
            ;;
        linux|wsl)
            if pkg-config --exists boost 2>/dev/null; then
                boost_ok=true
            elif dpkg -s libboost-all-dev &>/dev/null 2>&1; then
                boost_ok=true
            elif rpm -q boost-devel &>/dev/null 2>&1; then
                boost_ok=true
            elif pacman -Q boost &>/dev/null 2>&1; then
                boost_ok=true
            fi
            ;;
    esac
    if $boost_ok; then
        success "Boost"
    else
        error "Boost — not found"
        missing=$((missing + 1))
    fi

    # CAF
    local caf_ok=false
    case "$GJ_OS" in
        macos)
            if brew list caf &>/dev/null 2>&1; then caf_ok=true; fi
            ;;
        linux|wsl)
            if pkg-config --exists libcaf_core 2>/dev/null; then
                caf_ok=true
            elif dpkg -s libcaf-dev &>/dev/null 2>&1; then
                caf_ok=true
            fi
            ;;
    esac
    # Also check for source-built CAF in deps/
    if [[ -d "$DEPS_DIR/actor-framework/build" ]]; then caf_ok=true; fi
    if $caf_ok; then
        success "CAF (C++ Actor Framework)"
    else
        error "CAF — not found"
        missing=$((missing + 1))
    fi

    # JACK
    local jack_ok=false
    case "$GJ_OS" in
        macos)
            if brew list jack &>/dev/null 2>&1; then jack_ok=true; fi
            ;;
        linux|wsl)
            if pkg-config --exists jack 2>/dev/null; then
                jack_ok=true
            elif dpkg -s libjack-jackd2-dev &>/dev/null 2>&1; then
                jack_ok=true
            fi
            ;;
    esac
    if $jack_ok; then
        success "JACK"
    else
        error "JACK — not found"
        missing=$((missing + 1))
    fi

    # VST3 SDK
    if [[ -d "$DEPS_DIR/vst3sdk/build" ]]; then
        success "VST3 SDK"
    else
        error "VST3 SDK — not built"
        missing=$((missing + 1))
    fi

    return "$missing"
}

# ─── Compiler Selection ──────────────────────────────────────────────

read_cmake_cache_value() {
    local cache_file="$1" cache_key="$2"
    sed -n "s#^${cache_key}:\\(FILEPATH\\|STRING\\|UNINITIALIZED\\)=##p" "$cache_file" | head -n 1
}

normalize_compiler_path() {
    local compiler_path="$1"
    [[ -n "$compiler_path" ]] || return 0

    if [[ "$compiler_path" == /* ]]; then
        echo "$compiler_path"
        return 0
    fi

    command -v "$compiler_path" 2>/dev/null || echo "$compiler_path"
}

resolve_compiler_selection() {
    GJ_CMAKE_C_COMPILER=""
    GJ_CMAKE_CXX_COMPILER=""

    case "$COMPILER" in
        system)
            GJ_CMAKE_C_COMPILER="$(command -v cc || true)"
            GJ_CMAKE_CXX_COMPILER="$(command -v c++ || true)"
            ;;
        gcc)
            if [[ "$GJ_OS" == "macos" ]]; then
                if ! command -v brew &>/dev/null || ! brew list gcc &>/dev/null 2>&1; then
                    error "GNU GCC is not installed. Install it with Homebrew: brew install gcc"
                    exit 1
                fi

                local gcc_prefix
                gcc_prefix="$(brew --prefix gcc)"
                GJ_CMAKE_C_COMPILER="$(find "$gcc_prefix/bin" -maxdepth 1 -type f -name 'gcc-[0-9]*' | sort -V | tail -n 1)"
                GJ_CMAKE_CXX_COMPILER="$(find "$gcc_prefix/bin" -maxdepth 1 -type f -name 'g++-[0-9]*' | sort -V | tail -n 1)"
            else
                GJ_CMAKE_C_COMPILER="$(command -v gcc || true)"
                GJ_CMAKE_CXX_COMPILER="$(command -v g++ || true)"
            fi
            ;;
        clang)
            GJ_CMAKE_C_COMPILER="$(command -v clang || true)"
            GJ_CMAKE_CXX_COMPILER="$(command -v clang++ || true)"
            ;;
        llvm)
            if [[ "$GJ_OS" == "macos" ]]; then
                if ! command -v brew &>/dev/null || ! brew list llvm &>/dev/null 2>&1; then
                    error "LLVM is not installed. Install it with Homebrew: brew install llvm"
                    exit 1
                fi

                local llvm_prefix
                llvm_prefix="$(brew --prefix llvm)"
                GJ_CMAKE_C_COMPILER="$llvm_prefix/bin/clang"
                GJ_CMAKE_CXX_COMPILER="$llvm_prefix/bin/clang++"
            else
                GJ_CMAKE_C_COMPILER="$(command -v clang || true)"
                GJ_CMAKE_CXX_COMPILER="$(command -v clang++ || true)"
            fi
            ;;
        *)
            error "Unsupported compiler selection: $COMPILER"
            exit 1
            ;;
    esac

    if [[ -z "$GJ_CMAKE_C_COMPILER" || -z "$GJ_CMAKE_CXX_COMPILER" ]]; then
        error "Unable to resolve compiler executables for '$COMPILER'."
        exit 1
    fi

    if [[ ! -x "$GJ_CMAKE_C_COMPILER" || ! -x "$GJ_CMAKE_CXX_COMPILER" ]]; then
        error "Resolved compiler executables are not runnable."
        step "C compiler:   $GJ_CMAKE_C_COMPILER"
        step "C++ compiler: $GJ_CMAKE_CXX_COMPILER"
        exit 1
    fi
}

reset_cmake_toolchain_cache_if_needed() {
    local cache_file="$BUILD_DIR/CMakeCache.txt"
    [[ -f "$cache_file" ]] || return 0

    local cached_c_compiler cached_cxx_compiler
    cached_c_compiler="$(normalize_compiler_path "$(read_cmake_cache_value "$cache_file" "CMAKE_C_COMPILER")")"
    cached_cxx_compiler="$(normalize_compiler_path "$(read_cmake_cache_value "$cache_file" "CMAKE_CXX_COMPILER")")"

    if [[ -z "$cached_c_compiler" || -z "$cached_cxx_compiler" ]]; then
        return 0
    fi

    if [[ "$cached_c_compiler" != "$GJ_CMAKE_C_COMPILER" || "$cached_cxx_compiler" != "$GJ_CMAKE_CXX_COMPILER" ]]; then
        warn "Build directory was configured with a different compiler."
        step "Resetting cached CMake toolchain files in ${BOLD}$BUILD_DIR${RESET}"
        rm -f "$cache_file"
        rm -rf "$BUILD_DIR/CMakeFiles"
    fi
}

# ─── Build Configuration Prompts ─────────────────────────────────────

prompt_build_config() {
    header "Build Configuration"

    BUILD_TYPE=$(prompt_choice "Build type" "Debug" \
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

    local asan_default
    if [[ "$BUILD_TYPE" == "Debug" ]]; then asan_default="Y"; else asan_default="N"; fi
    if prompt_yn "Enable AddressSanitizer?" "$asan_default"; then
        ENABLE_ASAN="ON"
    else
        ENABLE_ASAN="OFF"
    fi

    COMPILER=$(prompt_choice "Compiler" "system" \
        "system" "gcc" "clang" "llvm")

    BUILD_DIR=$(prompt_input "Build directory" "$DEFAULT_BUILD_DIR")

    resolve_compiler_selection

    echo ""
    info "Summary:"
    step "Build type:  ${BOLD}$BUILD_TYPE${RESET}"
    step "ASan:        ${BOLD}$ENABLE_ASAN${RESET}"
    step "Compiler:    ${BOLD}$COMPILER${RESET}"
    step "C compiler:  ${BOLD}$GJ_CMAKE_C_COMPILER${RESET}"
    step "C++ compiler:${BOLD}$GJ_CMAKE_CXX_COMPILER${RESET}"
    step "Build dir:   ${BOLD}$BUILD_DIR${RESET}"
    step "Platform:    ${BOLD}${GJ_OS} (${GJ_ARCH})${RESET}"
    echo ""
}

# ─── CMake Configure ─────────────────────────────────────────────────

run_cmake_configure() {
    header "Configuring (CMake)"

    mkdir -p "$BUILD_DIR"
    reset_cmake_toolchain_cache_if_needed

    local cmake_args=(
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
        -DGJ_BASE_DIR="$DEPS_DIR"
        -DGJ_ENABLE_ASAN="$ENABLE_ASAN"
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        -DCMAKE_C_COMPILER="$GJ_CMAKE_C_COMPILER"
        -DCMAKE_CXX_COMPILER="$GJ_CMAKE_CXX_COMPILER"
    )

    case "$GJ_OS" in
        macos)
            if command -v brew &>/dev/null; then
                local caf_prefix qt_prefix
                caf_prefix="$(brew --prefix caf 2>/dev/null || true)"
                qt_prefix="$(brew --prefix qt 2>/dev/null || true)"
                [[ -n "$caf_prefix" ]] && cmake_args+=(-DCAF_ROOT="$caf_prefix")
                [[ -n "$qt_prefix" ]]  && cmake_args+=(-DCMAKE_PREFIX_PATH="$qt_prefix")
            fi
            ;;
        linux|wsl)
            if [[ -d "$DEPS_DIR/actor-framework/build" ]]; then
                cmake_args+=(-DCAF_ROOT="$DEPS_DIR/actor-framework/build")
            else
                cmake_args+=(-DCAF_ROOT=/usr)
            fi
            ;;
    esac

    step "cmake ${cmake_args[*]} $SRC_DIR"
    echo ""

    cmake -S "$SRC_DIR" -B "$BUILD_DIR" "${cmake_args[@]}"

    success "Configuration complete."
}

# ─── CMake Build ──────────────────────────────────────────────────────

run_cmake_build() {
    header "Building GrooveJr"

    local jobs
    jobs="$(cpu_count)"
    step "Compiling with $jobs parallel job(s)..."
    echo ""

    cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel "$jobs"

    success "Build complete!"
}

# ─── Main ─────────────────────────────────────────────────────────────

main() {
    banner "Build"

    detect_platform
    detect_pkg_manager

    info "Platform: ${BOLD}${GJ_OS}${RESET} (${GJ_ARCH})"

    if [[ "$GJ_OS" == "windows" ]]; then
        warn "Native Windows is not yet fully supported."
        warn "Please use WSL (Windows Subsystem for Linux) and re-run this script."
        exit 1
    fi

    # ── Dependency check ──
    local deps_missing=0
    check_dependencies || deps_missing=$?

    if [[ "$deps_missing" -gt 0 ]]; then
        echo ""
        warn "$deps_missing dependency/dependencies missing."
        if prompt_yn "Run the installer now?" Y; then
            echo ""
            if [[ "$GJ_AUTO_YES" == "1" ]]; then
                "$SCRIPT_DIR/install.sh" -y
            else
                "$SCRIPT_DIR/install.sh"
            fi
            echo ""
            header "Re-checking Dependencies"
            local recheck=0
            check_dependencies || recheck=$?
            if [[ "$recheck" -gt 0 ]]; then
                error "Some dependencies are still missing. Please install them manually."
                exit 1
            fi
        else
            warn "Continuing without all dependencies — build may fail."
        fi
    else
        echo ""
        success "All dependencies found!"
    fi

    # ── Build configuration ──
    prompt_build_config

    if ! prompt_yn "Start build?" Y; then
        info "Build cancelled."
        exit 0
    fi

    # ── Configure & build ──
    run_cmake_configure
    run_cmake_build

    # ── Done ──
    echo ""
    echo -e "${BOLD}╔══════════════════════════════════════════╗${RESET}"
    printf  "${BOLD}║${RESET}  ${GREEN}%-38s${RESET}  ${BOLD}║${RESET}\n" "Build succeeded!"
    echo -e "${BOLD}╚══════════════════════════════════════════╝${RESET}"
    echo ""

    if [[ "$GJ_OS" == "macos" ]]; then
        info "App bundle: ${BOLD}${BUILD_DIR}/GrooveJr.app${RESET}"
        info "Run:        ${BOLD}open ${BUILD_DIR}/GrooveJr.app${RESET}"
    else
        info "Binary: ${BOLD}${BUILD_DIR}/GrooveJr${RESET}"
        info "Run:    ${BOLD}${BUILD_DIR}/GrooveJr${RESET}"
    fi
}

main
