# GrooveJr
### Music Library + Player Built With
- CAF (C++ Actor Framework)
- Qt
- portaudio
- libsndfile

---

### MacOs Build (Dev Build)

- ```
  brew install cmake jack portaudio libsndfile  

- build the following from source and update CMakeLists.txt files with build locations:
  - [CAF](https://www.actor-framework.org/)
  - [Qt](https://www.qt.io/)
  - [Vst3](https://github.com/steinbergmedia/vst3sdk)

- ```
  git clone git@github.com:nathanielBellamy/GrooveJr.git
  cd GrooveJr/src
  mkdir cmake-build-debug
  cd cmake-build-debug
  cmake -DCMAKE_BUILD_TYPE=RELEASE ../
  cmake --build .

___

- jr to [GrooveSprings](https://github.com/nathanielBellamy/GrooveSprings)