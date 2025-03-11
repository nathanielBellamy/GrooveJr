# GrooveJr
### Music Library, Player, and VST3 Host Built With
- portaudio
- libsndfile
- vst3sdk
- CAF (C++ Actor Framework)
- Qt

---

### MacOs Build (Dev Build)

- ```bash
  brew install boost cmake jack portaudio libsndfile
  git clone git@github.com:nathanielBellamy/GrooveJr.git 
  cd GrooveJr/src
  vim CMakeLists.txt
  ```
  
  - it may be helpful to run: 
    ```bash
      brew services restart jack
    ```
- update `GJ_BASE_DIR` to be the directory containing the `GrooveJr` repo

- build the following from source in `GJ_BASE_DIR`
  - [CAF](https://www.actor-framework.org/)
  - [Qt](https://www.qt.io/)
  - [Vst3](https://github.com/steinbergmedia/vst3sdk)
    - On macOS:
      - after installing XCode from the app store, if cmake has trouble finding it while
        building vst3sdk, run
        ```bash 
        sudo xcode-select --reset
        ```
- by this point, `GJ_BASE_DIR` should include the following directories:
```
actor-framework/
GrooveJr/
qt6/
qt6-build/
vst3sdk/
```

- build it:
```
  cd GrooveJr/src
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ../
  cmake --build .
```
___

- jr to [GrooveSprings](https://github.com/nathanielBellamy/GrooveSprings)