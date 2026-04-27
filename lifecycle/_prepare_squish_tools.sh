#!/usr/bin/env bash
set -euo pipefail

if [[ -z "${GITHUB_ENV:-}" ]] || [[ -z "${GITHUB_PATH:-}" ]]; then
    echo "GITHUB_ENV and GITHUB_PATH must be set so Squish tool paths can be shared with later workflow steps." >&2
    exit 1
fi

resolve_squish_prefix() {
    if [[ -n "${SQUISH_PREFIX:-}" ]] && [[ -x "${SQUISH_PREFIX}/bin/squishserver" ]] && [[ -x "${SQUISH_PREFIX}/bin/squishrunner" ]]; then
        echo "${SQUISH_PREFIX}"
        return 0
    fi

    local server_path=""
    local runner_path=""

    if command -v squishserver &>/dev/null; then
        server_path="$(command -v squishserver)"
    fi

    if command -v squishrunner &>/dev/null; then
        runner_path="$(command -v squishrunner)"
    fi

    if [[ -n "${server_path}" ]] && [[ -n "${runner_path}" ]]; then
        echo "$(cd "$(dirname "${server_path}")/.." && pwd)"
        return 0
    fi

    return 1
}

download_squish_archive() {
    if [[ -z "${SQUISH_ARCHIVE_URL:-}" ]]; then
        echo "Unable to find Squish tools. Set SQUISH_PREFIX, add squishserver/squishrunner to PATH, or set the squish_archive_url secret." >&2
        return 1
    fi

    local work_dir archive_path archive_name extract_dir resolved_prefix
    work_dir="${RUNNER_TEMP:-${TMPDIR:-/tmp}}/squish-tools"
    rm -rf "${work_dir}"
    mkdir -p "${work_dir}"

    archive_name="$(basename "${SQUISH_ARCHIVE_URL%%\?*}")"
    archive_name="${archive_name:-squish-tools.zip}"
    archive_path="${work_dir}/${archive_name}"
    extract_dir="${work_dir}/extract"
    mkdir -p "${extract_dir}"

    curl -LfsS "${SQUISH_ARCHIVE_URL}" -o "${archive_path}"

    case "${archive_path}" in
        *.zip)
            ditto -x -k "${archive_path}" "${extract_dir}"
            ;;
        *.tar.gz|*.tgz)
            tar -xzf "${archive_path}" -C "${extract_dir}"
            ;;
        *.tar.xz)
            tar -xJf "${archive_path}" -C "${extract_dir}"
            ;;
        *)
            echo "Unsupported Squish archive format: ${archive_name}. Use .zip, .tar.gz, .tgz, or .tar.xz." >&2
            return 1
            ;;
    esac

    resolved_prefix="$(
        find "${extract_dir}" -type f -path '*/bin/squishserver' -print -quit |
        while IFS= read -r server_bin; do
            dirname "$(dirname "${server_bin}")"
        done
    )"

    if [[ -z "${resolved_prefix}" ]] || [[ ! -x "${resolved_prefix}/bin/squishrunner" ]]; then
        echo "Downloaded Squish archive does not contain bin/squishserver and bin/squishrunner." >&2
        return 1
    fi

    echo "${resolved_prefix}"
}

main() {
    local squish_prefix

    if ! squish_prefix="$(resolve_squish_prefix)"; then
        squish_prefix="$(download_squish_archive)"
    fi

    echo "SQUISH_PREFIX=${squish_prefix}" >> "${GITHUB_ENV}"
    echo "${squish_prefix}/bin" >> "${GITHUB_PATH}"
}

main
