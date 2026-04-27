#!/usr/bin/env bash
set -euo pipefail

ACTION="${1:-}"
CAPTURE_DIR="${2:-}"
VIDEO_FILE="${3:-}"
FRAME_DELAY_SECONDS="${SQUISH_RECORDING_FRAME_DELAY_SECONDS:-1}"
FRAME_RATE="${SQUISH_RECORDING_FRAME_RATE:-2}"

require_capture_dir() {
    if [[ -z "${CAPTURE_DIR}" ]]; then
        echo "A capture directory path is required." >&2
        exit 1
    fi
}

write_metadata() {
    local message="$1"
    mkdir -p "${CAPTURE_DIR}"
    cat > "${CAPTURE_DIR}/README.txt" <<EOF
Squish screen recording artifact

${message}
EOF
}

start_recording() {
    require_capture_dir

    if [[ -z "${GITHUB_ENV:-}" ]]; then
        echo "GITHUB_ENV must be set so the recording PID can be shared with later workflow steps." >&2
        exit 1
    fi

    if ! command -v screencapture >/dev/null 2>&1; then
        echo "screencapture is required to capture the Squish recording on macOS." >&2
        exit 1
    fi

    mkdir -p "${CAPTURE_DIR}"
    write_metadata "Frames are captured as PNG files. A MP4 is added when ffmpeg is available on the runner."

    (
        frame=0
        while true; do
            printf -v frame_file "%s/frame-%06d.png" "${CAPTURE_DIR}" "${frame}"
            screencapture -x "${frame_file}" || rm -f "${frame_file}"
            frame=$((frame + 1))
            sleep "${FRAME_DELAY_SECONDS}"
        done
    ) > "${CAPTURE_DIR}/capture.log" 2>&1 &

    echo "SQUISH_RECORDING_PID=$!" >> "${GITHUB_ENV}"
}

stop_recording() {
    require_capture_dir

    mkdir -p "${CAPTURE_DIR}"

    if [[ -n "${SQUISH_RECORDING_PID:-}" ]]; then
        kill "${SQUISH_RECORDING_PID}" 2>/dev/null || true
    fi

    if ! compgen -G "${CAPTURE_DIR}/frame-*.png" > /dev/null; then
        write_metadata "No screenshots were captured during the Squish run."
        exit 0
    fi

    if [[ -n "${VIDEO_FILE}" ]] && command -v ffmpeg >/dev/null 2>&1; then
        ffmpeg -y \
            -framerate "${FRAME_RATE}" \
            -pattern_type glob \
            -i "${CAPTURE_DIR}/frame-*.png" \
            -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2,format=yuv420p" \
            "${VIDEO_FILE}"
        write_metadata "The PNG frame sequence and squish-screen-recording.mp4 were captured during the Squish run."
        exit 0
    fi

    write_metadata "The PNG frame sequence was captured during the Squish run. Install ffmpeg on the runner to also produce squish-screen-recording.mp4."
}

case "${ACTION}" in
    start)
        start_recording
        ;;
    stop)
        stop_recording
        ;;
    *)
        echo "Usage: $0 <start|stop> <capture-dir> [video-file]" >&2
        exit 1
        ;;
esac
