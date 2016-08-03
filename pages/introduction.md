---
layout: page
title: Introduction
tagline: HEVC v2 / ITU-T H.265 v2 (10/2014) decoding process analyzer.
---

#### Deviation from ITU-T H.265 v2 (10/2014) and v3 (04/2015) specifications:

* It is specified that **escapeDataPresent** flag should be reset
every block but technical intent was to reset it every subblock.
This is considered as a [BUG](https://hevc.hhi.fraunhofer.de/trac/hevc/ticket/1391)
in text and implementation resets it every subblock.

---

#### Author

[Wlodzimierz Lipert](https://github.com/vlad83)

#### Contributors

* [Michal Bartkowiak](https://github.com/miszak)
* [Karol Wozniak](https://github.com/attugit)

