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

GrooveJr ships with repo-local scripts in `./lifecycle/` for installing dependencies, building, running, and testing the
app. These scripts are the recommended entrypoints for local development on macOS, Linux, and WSL. Native Windows is not
currently supported.

All of the scripts support `-h`/`--help`, and the interactive scripts support `-y`/`--yes` to accept defaults without
prompting.

| Script                   | Purpose                                                   | Notes                                                                                                                                                        |
|--------------------------|-----------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `./lifecycle/install.sh` | Install system dependencies and build source dependencies | Detects `brew`, `apt`, `dnf`, or `pacman`; installs packages; installs CAF from a package manager when possible; clones and builds the VST3 SDK in `./deps/` |
| `./lifecycle/build.sh`   | Configure and build GrooveJr                              | Checks dependencies first, offers to run `install.sh`, then configures CMake and builds the app                                                              |
| `./lifecycle/run.sh`     | Launch an existing build                                  | Starts `GrooveJr.app` on macOS or the `GrooveJr` binary on Linux/WSL                                                                                         |
| `./lifecycle/test.sh`    | Build and run the test suite                              | Configures a separate `./build-test/` tree with `-DGJ_BUILD_TESTS=ON` and runs tests through CTest                                                           |
| `./lifecycle/_common.sh` | Shared helpers for the lifecycle scripts                  | Not intended to be run directly                                                                                                                              |

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
- App icon assets live in `./src/resource/icons/`; CMake bundles the macOS `.icns`, uses the Windows `.ico`, and the Qt app loads the shared PNG at runtime.

### Useful Non-Interactive Examples

```bash
./lifecycle/install.sh -y
./lifecycle/build.sh -y
./lifecycle/run.sh --build-dir ./src/build
./lifecycle/test.sh -y --filter '*Math*' --verbose
```

___

### MacOs Build (Dev Build)

- ```bash
  brew install boost cmake jack fftw taglib blackhole-2ch libsndfile sqlite
  git clone git@github.com:nathanielBellamy/GrooveJr.git 
  ```

- it may be helpful to run:
  ```bash
    brew services restart jack
  ```
- blackhole-2ch is required for devices without built-in input and/or output audio devices
    - blackhole-2ch creates virtual inputs and outputs
    - without these the jack server will not be able to run

---

- jr to [GrooveSprings](https://github.com/nathanielBellamy/GrooveSprings)
