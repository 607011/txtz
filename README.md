# txtz

**Short string compression**

THIS IS WORK IN PROGRESS. DO NOT EXPECT ANYTHING USEFUL IN THIS REPO AT THE MOMENT.


## Prerequisites

- C++ compiler (GCC, Clang)
- CMake 3.10 or later

## Build 

### Linux

Ubuntu

```
sudo apt install g++ libboost-all-dev
```

```
git clone https://github.com/607011/txtz.git
cd txtz
git submodule update init
git submodule update --remote --merge
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### macOS

```
git clone https://github.com/607011/txtz.git
cd txtz
git submodule update init
git submodule update --remote --merge
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```


### Windows

In Visual Studio Developer Console:

```
git clone https://github.com/607011/txtz.git
cd txtz
git submodule update init
git submodule update --remote --merge
md build
cd build
cmake ..
cmake --build . --config Release
```


## Usage

TODO

## License

See (LICENSE).
