#!/usr/bin/env bash
# lifecycle/_common.sh — Shared utilities for GrooveJr lifecycle scripts
#
# Source this file from other lifecycle scripts; do not run directly.

set -euo pipefail

# ─── Project Configuration ───────────────────────────────────────────

VST3_SDK_VERSION="${VST3_SDK_VERSION:-v3.7.12_build_20}"

LIFECYCLE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$LIFECYCLE_DIR/.." && pwd)"
DEPS_DIR="${REPO_ROOT}/deps"
SRC_DIR="${REPO_ROOT}/src"
DEFAULT_BUILD_DIR="${REPO_ROOT}/build"

# ─── Terminal Colors ─────────────────────────────────────────────────

if [[ -t 1 ]]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    BLUE='\033[0;34m'
    BOLD='\033[1m'
    DIM='\033[2m'
    RESET='\033[0m'
else
    RED='' GREEN='' YELLOW='' BLUE='' BOLD='' DIM='' RESET=''
fi

# ─── Output Helpers ──────────────────────────────────────────────────

info()    { echo -e "${BLUE}ℹ${RESET}  $*"; }
success() { echo -e "${GREEN}✓${RESET}  $*"; }
warn()    { echo -e "${YELLOW}⚠${RESET}  $*"; }
error()   { echo -e "${RED}✗${RESET}  $*" >&2; }
header()  { echo -e "\n${BOLD}── $* ──${RESET}"; }
step()    { echo -e "  ${DIM}→${RESET} $*"; }

banner() {
    local title="GrooveJr · $1"
    echo ""
    echo -e "${BOLD}╔══════════════════════════════════════════╗${RESET}"
    printf  "${BOLD}║${RESET}  %-38s  ${BOLD}║${RESET}\n" "$title"
    echo -e "${BOLD}╚══════════════════════════════════════════╝${RESET}"
    echo ""
}

# ─── Platform Detection ──────────────────────────────────────────────

detect_platform() {
    local uname_s
    uname_s="$(uname -s)"

    case "$uname_s" in
        Darwin)         GJ_OS="macos" ;;
        Linux)
            if grep -qi microsoft /proc/version 2>/dev/null; then
                GJ_OS="wsl"
            else
                GJ_OS="linux"
            fi
            ;;
        MINGW*|MSYS*|CYGWIN*)
            GJ_OS="windows"
            ;;
        *)
            error "Unsupported operating system: $uname_s"
            exit 1
            ;;
    esac

    GJ_ARCH="$(uname -m)"
    case "$GJ_ARCH" in
        x86_64|amd64)   GJ_ARCH="x86_64" ;;
        arm64|aarch64)  GJ_ARCH="arm64"  ;;
    esac

    export GJ_OS GJ_ARCH
}

detect_pkg_manager() {
    if command -v brew &>/dev/null; then
        GJ_PKG_MGR="brew"
    elif command -v apt-get &>/dev/null; then
        GJ_PKG_MGR="apt"
    elif command -v dnf &>/dev/null; then
        GJ_PKG_MGR="dnf"
    elif command -v pacman &>/dev/null; then
        GJ_PKG_MGR="pacman"
    else
        GJ_PKG_MGR=""
        warn "No supported package manager found (brew, apt, dnf, pacman)"
    fi

    export GJ_PKG_MGR
}

# Number of CPU cores for parallel builds
cpu_count() {
    if command -v nproc &>/dev/null; then
        nproc
    elif sysctl -n hw.ncpu &>/dev/null 2>&1; then
        sysctl -n hw.ncpu
    else
        echo 4
    fi
}

# ─── Interactive Prompts ─────────────────────────────────────────────
# Set GJ_AUTO_YES=1 or pass -y to scripts to accept all defaults.

GJ_AUTO_YES="${GJ_AUTO_YES:-0}"

# Yes/No prompt.  Usage: prompt_yn "Question?" Y
prompt_yn() {
    local question="$1"
    local default="${2:-Y}"

    if [[ "$GJ_AUTO_YES" == "1" ]]; then
        [[ "$default" =~ ^[Yy] ]]
        return
    fi

    local hint
    if [[ "$default" =~ ^[Yy] ]]; then hint="Y/n"; else hint="y/N"; fi

    local answer
    read -rp "$(echo -e "   ${BOLD}$question${RESET} [$hint]: ")" answer
    answer="${answer:-$default}"
    [[ "$answer" =~ ^[Yy] ]]
}

# Choice prompt.  Usage: val=$(prompt_choice "Build type" "Debug" "Debug" "Release")
prompt_choice() {
    local prompt_text="$1" default="$2"
    shift 2
    local options=("$@")

    if [[ "$GJ_AUTO_YES" == "1" ]]; then
        echo "$default"
        return
    fi

    local options_display
    options_display=$(IFS=/; echo "${options[*]}")

    local answer
    read -rp "$(echo -e "   ${BOLD}$prompt_text${RESET} [$options_display] (${default}): ")" answer
    echo "${answer:-$default}"
}

# Free-form prompt.  Usage: val=$(prompt_input "Build dir" "/default/path")
prompt_input() {
    local prompt_text="$1" default="$2"

    if [[ "$GJ_AUTO_YES" == "1" ]]; then
        echo "$default"
        return
    fi

    local answer
    read -rp "$(echo -e "   ${BOLD}$prompt_text${RESET} (${default}): ")" answer
    echo "${answer:-$default}"
}
