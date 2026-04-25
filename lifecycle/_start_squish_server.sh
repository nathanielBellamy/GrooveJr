#!/usr/bin/env bash
set -euo pipefail

if [[ -z "${SQUISH_PREFIX:-}" ]]; then
    echo "SQUISH_PREFIX must be set before starting squishserver." >&2
    exit 1
fi

if [[ -z "${GITHUB_ENV:-}" ]]; then
    echo "GITHUB_ENV must be set so the Squish server PID can be shared with later workflow steps." >&2
    exit 1
fi

LOG_FILE="${1:-/tmp/groovejr-squishserver.log}"
SQUISH_SERVER_PORT="${SQUISH_SERVER_PORT:-4322}"

"${SQUISH_PREFIX}/bin/squishserver" > "${LOG_FILE}" 2>&1 &
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
