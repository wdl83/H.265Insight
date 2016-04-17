# H.265Insight
ITU-T H.265 v2 (10/2014) decoding process analyzer.

**i265** implements decoding process as defined by **ITU-T H.265** (http://www.itu.int/rec/T-REC-H.265-201504-I/en).

## Requirements

- C++11 conformant compiler. Complete project was built with GCC 5.3.1 and CLANG 3.6.2 before release;
- CMake;
- POSIX API and GNU extensions, required by TerminationTools (backtrace pretty printer, can be easily disabled);

## Installation
```
git clone https://github.com/vlad83/H.265Insight
mkdir obj
cd obj
cmake -DCMAKE_BUILD_TYPE=RELEASE ../H.265Insight
make
```
to use CLANG
```
cmake -DUSE_CLANG=ON -DCMAKE_BUILD_TYPE=RELEASE ../H.265Insight
```

**i265** binary is located in obj/bin directory. For complete list of build time options see Flags.cmake

## Example

Directory "example" contains analysis files (*.log, *.yuv) from:
- bitstream: http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/VPSID_A_VIDYO_2.zip
- command line arguments: ./bin/i265 -i VPSID_A_VIDYO_2.bit -n 3 -l all -o 2

## author:
Wlodzimierz Lipert https://github.com/vlad83
## co-author:
Michal Bartkowiak https://github.com/miszak
