#!/usr/bin/env bash
set -euo pipefail

if [[ -z "${GITHUB_ENV:-}" ]]; then
    echo "GITHUB_ENV must be set so the Squish server PID can be shared with later workflow steps." >&2
    exit 1
fi

DEFAULT_LOG_DIR="${RUNNER_TEMP:-${TMPDIR:-/tmp}}"
LOG_FILE="${1:-${DEFAULT_LOG_DIR}/groovejr-squishserver.log}"
SQUISH_SERVER_PORT="${SQUISH_SERVER_PORT:-4322}"

find_squish_server() {
    if [[ -n "${SQUISH_PREFIX:-}" ]] && [[ -x "${SQUISH_PREFIX}/bin/squishserver" ]]; then
        echo "${SQUISH_PREFIX}/bin/squishserver"
        return 0
    fi

    if command -v squishserver &>/dev/null; then
        command -v squishserver
        return 0
    fi

    echo "Unable to find squishserver. Set SQUISH_PREFIX or add squishserver to PATH." >&2
    exit 1
}

SQUISH_SERVER_BIN="$(find_squish_server)"

"${SQUISH_SERVER_BIN}" > "${LOG_FILE}" 2>&1 &
SQUISHSERVER_PID=$!
echo "SQUISHSERVER_PID=${SQUISHSERVER_PID}" >> "${GITHUB_ENV}"

for _ in $(seq 1 30); do
    if ! kill -0 "${SQUISHSERVER_PID}" 2>/dev/null; then
        cat "${LOG_FILE}" >&2 || true
        echo "squishserver exited before it was ready." >&2
        exit 1
    fi

    if nc -z 127.0.0.1 "${SQUISH_SERVER_PORT}" 2>/dev/null; then
        exit 0
    fi

    sleep 1
done

cat "${LOG_FILE}" >&2 || true
echo "Timed out waiting for squishserver to listen on port ${SQUISH_SERVER_PORT}." >&2
exit 1
