# Squish scaffolding

This directory holds the starter Squish suite for GrooveJr's Qt UI.

## Prerequisites

- A local GrooveJr build in `./src/build` or another CMake build directory
- A local Squish for Qt installation with `squishrunner` available on `PATH`, via `SQUISH_PREFIX`, or passed to `./lifecycle/squish.sh --runner`

## Layout

- `suite_groovejr/` — the starter Squish suite
- `suite_groovejr/shared/scripts/common.py` — shared helpers for resolving and launching the app under test
- `suite_groovejr/shared/scripts/names.py` — starter symbolic selectors for GrooveJr widgets
- `suite_groovejr/tst_template/` — a copyable placeholder testcase
- `suite_groovejr/tst_menu_structure/` — verifies the File and Music menus expose the expected starter actions
- `suite_groovejr/tst_music_library_sections/` — verifies the music-library headers are visible and the Files/Queue/Cache views toggle correctly

## Run the scaffolded suite

```bash
./lifecycle/build.sh -y
./lifecycle/squish.sh -y
```

You can override the defaults when needed:

```bash
./lifecycle/squish.sh \
  --build-dir ./src/build \
  --suite-dir ./tests/squish/suite_groovejr \
  --testcase tst_template
```

## Adding real tests

1. Copy `suite_groovejr/tst_template` to a new `tst_<name>` directory
2. Update `suite_groovejr/suite.conf` so the new testcase is listed in `TEST_CASES`
3. Add shared selectors to `shared/scripts/names.py`
4. Add shared launch and synchronization helpers to `shared/scripts/common.py` as the suite grows

Use the Squish IDE to record new interactions if you want to build out the object map and selectors visually.

## CI

GitHub Actions E2E runs use `.github/workflows/e2e.yml`.

- PR runs build GrooveJr and then execute the Squish suite
- Push builds reuse the app bundle produced by `.github/workflows/build.yml`
- The Squish job runs on a GitHub-hosted macOS runner and still requires Squish for Qt to be installed there
- Make `squishserver` and `squishrunner` available on the runner either via `PATH` or by setting the repo/org variable `SQUISH_PREFIX`
- If the hosted runner does not already have Squish installed, set `SQUISH_ARCHIVE_URL` (preferred) or legacy `squish_archive_url` as a repo/org variable pointing to a pre-extracted Squish archive with `bin/squishserver` and `bin/squishrunner`
- E2E uploads a `squish-screen-recording-artifact` containing PNG capture frames and, when `ffmpeg` is available on the runner, a `squish-screen-recording.mp4`
- For future audio-output validation on macOS runners, `blackhole-2ch` is the most likely path for giving JACK stable virtual input/output ports when no physical audio hardware is available
