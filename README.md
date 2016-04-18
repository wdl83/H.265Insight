# H.265Insight
ITU-T H.265 v2 (10/2014) decoding process analyzer.

**i265** implements decoding process as defined by **ITU-T H.265** (http://www.itu.int/rec/T-REC-H.265-201504-I/en).

[![Build Status](https://api.travis-ci.org/vlad83/H.265Insight.svg?branch=master)](https://travis-ci.org/vlad83/H.265Insight)

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

**i265** binary is located in obj/bin directory. For complete list of build time options see Flags.cmake

## Example

Directory **example** contains analysis files for:
 http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/VPSID_A_VIDYO_2.zip
 ```
 ./bin/i265 -i VPSID_A_VIDYO_2.bit -n 3 -l all -o 2
 ```
Intermediate and final YUVs are located in **example/YUVs**.

YUV images produced by **i265** can be viewed by **YUVViewer** (http://www.mlynki.wz.cz/english_i/einfoc23.htm).
Two output modes are supported **discrete** and **continuous**.

## author:
Wlodzimierz Lipert https://github.com/vlad83
## co-author:
Michal Bartkowiak https://github.com/miszak
