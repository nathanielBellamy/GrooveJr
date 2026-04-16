#!/usr/bin/env bash
# lifecycle/install.sh — GrooveJr Dependency Installer
#
# Detects the host OS and package manager, installs all required
# dependencies, and builds the VST3 SDK from source.
#
# Usage:
#   ./lifecycle/install.sh        # interactive
#   ./lifecycle/install.sh -y     # accept all defaults

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

trap 'echo ""; error "Installation interrupted."; exit 130' INT

# ─── Package Lists (per package manager) ─────────────────────────────
# CAF is handled separately below — it may need a source build on Linux.

BREW_PACKAGES=(cmake boost jack fftw taglib libsndfile sqlite qt)

APT_PACKAGES=(
    build-essential cmake git pkg-config
    libboost-all-dev
    libjack-jackd2-dev
    libfftw3-dev
    libtag1-dev
    libsndfile1-dev
    libsqlite3-dev
    qt6-base-dev qt6-base-dev-tools
    libgl-dev
)

DNF_PACKAGES=(
    gcc-c++ cmake git pkg-config
    boost-devel
    jack-audio-connection-kit-devel
    fftw-devel
    taglib-devel
    libsndfile-devel
    sqlite-devel
    qt6-qtbase-devel
    mesa-libGL-devel
)

PACMAN_PACKAGES=(
    base-devel cmake git pkg-config
    boost jack2 fftw taglib libsndfile sqlite
    qt6-base
)

# ─── System Package Installers ───────────────────────────────────────

install_brew_packages() {
    header "System packages (Homebrew)"
    local to_install=()
    for pkg in "${BREW_PACKAGES[@]}"; do
        if brew list "$pkg" &>/dev/null; then
            success "$pkg"
        else
            to_install+=("$pkg")
            step "$pkg (will install)"
        fi
    done

    if [[ ${#to_install[@]} -eq 0 ]]; then
        info "All Homebrew packages already installed."
        return 0
    fi

    if prompt_yn "Install ${#to_install[@]} package(s) via Homebrew?" Y; then
        brew install "${to_install[@]}"
        success "Homebrew packages installed."
    else
        warn "Skipped — build may fail if these are required."
    fi
}

install_apt_packages() {
    header "System packages (apt)"
    local to_install=()
    for pkg in "${APT_PACKAGES[@]}"; do
        if dpkg -s "$pkg" &>/dev/null 2>&1; then
            success "$pkg"
        else
            to_install+=("$pkg")
            step "$pkg (will install)"
        fi
    done

    if [[ ${#to_install[@]} -eq 0 ]]; then
        info "All apt packages already installed."
        return 0
    fi

    if prompt_yn "Install ${#to_install[@]} package(s) via apt? (requires sudo)" Y; then
        sudo apt-get update -qq
        sudo apt-get install -y "${to_install[@]}"
        success "apt packages installed."
    else
        warn "Skipped — build may fail if these are required."
    fi
}

install_dnf_packages() {
    header "System packages (dnf)"
    local to_install=()
    for pkg in "${DNF_PACKAGES[@]}"; do
        if rpm -q "$pkg" &>/dev/null 2>&1; then
            success "$pkg"
        else
            to_install+=("$pkg")
            step "$pkg (will install)"
        fi
    done

    if [[ ${#to_install[@]} -eq 0 ]]; then
        info "All dnf packages already installed."
        return 0
    fi

    if prompt_yn "Install ${#to_install[@]} package(s) via dnf? (requires sudo)" Y; then
        sudo dnf install -y "${to_install[@]}"
        success "dnf packages installed."
    else
        warn "Skipped — build may fail if these are required."
    fi
}

install_pacman_packages() {
    header "System packages (pacman)"
    local to_install=()
    for pkg in "${PACMAN_PACKAGES[@]}"; do
        if pacman -Q "$pkg" &>/dev/null 2>&1; then
            success "$pkg"
        else
            to_install+=("$pkg")
            step "$pkg (will install)"
        fi
    done

    if [[ ${#to_install[@]} -eq 0 ]]; then
        info "All pacman packages already installed."
        return 0
    fi

    if prompt_yn "Install ${#to_install[@]} package(s) via pacman? (requires sudo)" Y; then
        sudo pacman -S --noconfirm "${to_install[@]}"
        success "pacman packages installed."
    else
        warn "Skipped — build may fail if these are required."
    fi
}

install_system_packages() {
    case "$GJ_PKG_MGR" in
        brew)   install_brew_packages ;;
        apt)    install_apt_packages ;;
        dnf)    install_dnf_packages ;;
        pacman) install_pacman_packages ;;
        *)
            error "No supported package manager detected."
            error "Please install dependencies manually — see README.md."
            exit 1
            ;;
    esac
}

# ─── CAF (C++ Actor Framework) ───────────────────────────────────────
# Available in Homebrew; may or may not exist in Linux distro repos.
# Falls back to building from source when the package is unavailable.

caf_is_installed() {
    case "$GJ_PKG_MGR" in
        brew)   brew list caf &>/dev/null 2>&1 ;;
        apt)    dpkg -s libcaf-dev &>/dev/null 2>&1 ;;
        dnf)    rpm -q caf-devel &>/dev/null 2>&1 ;;
        pacman) pacman -Q caf &>/dev/null 2>&1 ;;
        *)      return 1 ;;
    esac
}

install_caf() {
    header "CAF (C++ Actor Framework)"

    if caf_is_installed; then
        success "CAF already installed via $GJ_PKG_MGR"
        return 0
    fi

    if [[ -d "$DEPS_DIR/actor-framework/build" ]]; then
        success "CAF already built from source at deps/actor-framework/"
        return 0
    fi

    # Try the package manager first
    local pkg_name=""
    case "$GJ_PKG_MGR" in
        brew)   pkg_name="caf" ;;
        apt)    pkg_name="libcaf-dev" ;;
        dnf)    pkg_name="caf-devel" ;;
    esac

    if [[ -n "$pkg_name" ]]; then
        step "Attempting to install '$pkg_name' via $GJ_PKG_MGR..."
        local install_ok=false
        case "$GJ_PKG_MGR" in
            brew)   brew install "$pkg_name" && install_ok=true ;;
            apt)    sudo apt-get install -y "$pkg_name" 2>/dev/null && install_ok=true ;;
            dnf)    sudo dnf install -y "$pkg_name" 2>/dev/null && install_ok=true ;;
        esac

        if $install_ok; then
            success "CAF installed via $GJ_PKG_MGR."
            return 0
        fi
        warn "CAF package not available in $GJ_PKG_MGR repos."
    fi

    # Fall back to source build
    info "CAF will be built from source."
    if ! prompt_yn "Proceed?" Y; then
        warn "Skipping CAF — build will fail without it."
        return 1
    fi

    mkdir -p "$DEPS_DIR"
    if [[ ! -d "$DEPS_DIR/actor-framework/.git" ]]; then
        step "Cloning CAF..."
        git clone --recursive \
            https://github.com/actor-framework/actor-framework.git \
            "$DEPS_DIR/actor-framework"
    fi

    step "Building CAF..."
    mkdir -p "$DEPS_DIR/actor-framework/build"
    (
        cd "$DEPS_DIR/actor-framework/build"
        cmake -DCMAKE_BUILD_TYPE=Release \
              -DCAF_ENABLE_EXAMPLES=OFF \
              -DCAF_ENABLE_TESTING=OFF \
              ..
        cmake --build . --parallel "$(cpu_count)"
    )
    success "CAF built successfully."
}

# ─── VST3 SDK (always built from source) ─────────────────────────────

install_vst3_sdk() {
    header "VST3 SDK ($VST3_SDK_VERSION)"

    if [[ -d "$DEPS_DIR/vst3sdk/build" ]]; then
        success "VST3 SDK already built at deps/vst3sdk/"
        if ! prompt_yn "Rebuild?" N; then
            return 0
        fi
    fi

    info "The VST3 SDK will be cloned and built from source."
    info "Destination: deps/vst3sdk/"
    if ! prompt_yn "Proceed?" Y; then
        warn "Skipping VST3 SDK — build will fail without it."
        return 1
    fi

    mkdir -p "$DEPS_DIR"

    if [[ ! -d "$DEPS_DIR/vst3sdk/.git" ]]; then
        step "Cloning VST3 SDK ${VST3_SDK_VERSION}..."
        git clone --recursive --branch "$VST3_SDK_VERSION" \
            https://github.com/steinbergmedia/vst3sdk.git "$DEPS_DIR/vst3sdk"
    else
        step "VST3 SDK source already present — checking out $VST3_SDK_VERSION..."
        (
            cd "$DEPS_DIR/vst3sdk"
            git fetch --tags
            git checkout "$VST3_SDK_VERSION"
            git submodule update --init --recursive
        )
    fi

    step "Building VST3 SDK..."
    mkdir -p "$DEPS_DIR/vst3sdk/build"
    (
        cd "$DEPS_DIR/vst3sdk/build"
        cmake -DCMAKE_BUILD_TYPE=Release \
              -DSMTG_ENABLE_VST3_HOSTING_EXAMPLES=OFF \
              -DSMTG_ENABLE_VST3_PLUGIN_EXAMPLES=OFF \
              -DSMTG_ENABLE_VSTGUI_SUPPORT=OFF \
              ..
        cmake --build . --config Release --parallel "$(cpu_count)" \
              --target sdk sdk_common sdk_hosting pluginterfaces
    )
    success "VST3 SDK built successfully."
}

# ─── Main ────────────────────────────────────────────────────────────

main() {
    banner "Install Dependencies"

    detect_platform
    detect_pkg_manager

    info "Platform:        ${BOLD}${GJ_OS}${RESET} (${GJ_ARCH})"
    info "Package manager: ${BOLD}${GJ_PKG_MGR:-none}${RESET}"

    if [[ "$GJ_OS" == "windows" ]]; then
        warn "Native Windows is not yet fully supported."
        warn "Please use WSL (Windows Subsystem for Linux) and re-run this script."
        exit 1
    fi

    install_system_packages
    install_caf
    install_vst3_sdk

    echo ""
    success "${BOLD}All dependencies installed!${RESET}"
    info "Build GrooveJr with: ${BOLD}./lifecycle/build.sh${RESET}"
}

main
