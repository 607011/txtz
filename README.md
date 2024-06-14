# txtz

**Short string compression**

THIS IS WORK IN PROGRESS. DO NOT EXPECT ANYTHING USEFUL IN THIS REPO AT THE MOMENT.



## Prerequisites

- Boost Libraries 1.71 or newer

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

To build JSON data and an #includable C++ file from a list with German forenames and their respective frequencies:

```
./mapbuilder -i ../data/de-vornamen+histo.txt \
  -g de-vornamen \
  --with-histo --histo-delim ';' \
  --phonemes '|' \
  --fill-missing-monograms abcdefghijklmnopqrtsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZäöüÄÖÜß’
```

The JSON and C++ file are saved as de-vornamen.json and de-vornamen.cpp.



## License

See (LICENSE).
