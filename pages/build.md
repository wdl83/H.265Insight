---
layout: page
title: Build
tagline: Instructions to build and verify i265 binary.
---

#### Requirements

* C++11 conformant compiler. Project is being [built with GCC 4.9.3/5.4.0/6.1.1 and CLANG 3.8.0](https://travis-ci.org/vlad83/H.265Insight)
* CMake
* POSIX API and GNU extensions, required by TerminationTools (backtrace pretty printer, can be easily disabled)

---

#### Repository

Project repository is hosted on [vlad83](https://github.com/vlad83/H.265Insight) GitHub account.

#### Installation

```
git clone https://github.com/vlad83/H.265Insight
mkdir obj
cd obj
cmake -DCMAKE_BUILD_TYPE=RELEASE ../H.265Insight
make
```

**i265** binary is located in `obj/bin` directory.
For complete list of build time options see [Flags.cmake](https://github.com/vlad83/H.265Insight/blob/master/Flags.cmake)

---

#### Continuous Integration

Verification of complete set of [JCTVC HEVC v1 conformance streams](http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/HEVC_v1).
is available in RELEASE [travis-ci builds](https://travis-ci.org/vlad83/H.265Insight?branch=master)

