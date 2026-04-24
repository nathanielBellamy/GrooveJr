# GrooveJr
### Music Library, Player, and VST3 Host Built With
- JACK
- libsndfile
- fftw
- vst3sdk
- CAF (C++ Actor Framework)
- Qt
- Boost
- taglib
- sqlite
- CMake
- C++17

---

### Lifecycle Scripts

GrooveJr ships with repo-local scripts in `./lifecycle/` for installing dependencies, building, running, and testing the app. These scripts are the recommended entrypoints for local development on macOS, Linux, and WSL. Native Windows is not currently supported.

All of the scripts support `-h`/`--help`, and the interactive scripts support `-y`/`--yes` to accept defaults without prompting.

| Script | Purpose | Notes |
| --- | --- | --- |
| `./lifecycle/install.sh` | Install system dependencies and build source dependencies | Detects `brew`, `apt`, `dnf`, or `pacman`; installs packages; installs CAF from a package manager when possible; clones and builds the VST3 SDK in `./deps/` |
| `./lifecycle/build.sh` | Configure and build GrooveJr | Checks dependencies first, offers to run `install.sh`, then configures CMake and builds the app |
| `./lifecycle/run.sh` | Launch an existing build | Starts `GrooveJr.app` on macOS or the `GrooveJr` binary on Linux/WSL |
| `./lifecycle/test.sh` | Build and run the test suite | Configures a separate `./build-test/` tree with `-DGJ_BUILD_TESTS=ON` and runs tests through CTest |
| `./lifecycle/_common.sh` | Shared helpers for the lifecycle scripts | Not intended to be run directly |

### Recommended Local Workflow

1. Install dependencies:
   ```bash
   ./lifecycle/install.sh
   ```
2. Build the app:
   ```bash
   ./lifecycle/build.sh
   ```
3. Run the app:
   ```bash
   ./lifecycle/run.sh
   ```
4. Run tests:
   ```bash
   ./lifecycle/test.sh
   ```

### Default Paths

- Source dependencies are placed in `./deps/`
- App builds default to `./src/build`
- Test builds default to `./build-test`

### Useful Non-Interactive Examples

```bash
./lifecycle/install.sh -y
./lifecycle/build.sh -y
./lifecycle/run.sh --build-dir ./src/build
./lifecycle/test.sh -y --filter '*Math*' --verbose
```
___

- jr to [GrooveSprings](https://github.com/nathanielBellamy/GrooveSprings)
