import os
import platform

source(findFile("scripts", "names.py"))


def repo_root():
    return os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))


def default_build_dir():
    return os.environ.get("GJ_SQUISH_BUILD_DIR", os.path.join(repo_root(), "src", "build"))


def groovejr_executable():
    override = os.environ.get("GJ_SQUISH_AUT")
    if override:
        return override

    build_dir = default_build_dir()
    system = platform.system()
    candidates = []

    if system == "Darwin":
        candidates.append(os.path.join(build_dir, "GrooveJr.app", "Contents", "MacOS", "GrooveJr"))
    elif system == "Windows":
        candidates.append(os.path.join(build_dir, "GrooveJr.exe"))
    else:
        candidates.append(os.path.join(build_dir, "GrooveJr"))

    for candidate in candidates:
        if os.path.exists(candidate):
            return candidate

    test.fatal(
        "Unable to locate the GrooveJr executable for Squish.",
        "Looked in: %s" % ", ".join(candidates),
    )
    return None


def start_groovejr():
    executable = groovejr_executable()
    test.log("Launching GrooveJr from %s" % executable)
    startApplication(executable)
    return wait_for_main_window()


def wait_for_main_window(timeout_ms=20000):
    return waitForObject(main_window(), timeout_ms)
