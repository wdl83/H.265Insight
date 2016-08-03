---
layout: page
title: Examples
tagline: Usage examples.
---

#### Examples

Directory **example** contains analysis files for [VPSID_A_VIDYO_2](http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/HEVC_v1/VPSID_A_VIDYO_2.zip):

```
./bin/i265 -i VPSID_A_VIDYO_2.bit -n 3 -l all -o 2
```

Intermediate and final YUVs are located in **example/YUVs**.

YUV images produced by **i265** can be viewed by [YUVViewer](http://www.mlynki.wz.cz/english_i/einfoc23.htm).
Two output modes are supported **discrete** and **continuous**.

