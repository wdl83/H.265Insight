# H.265Insight
ITU-T H.265 v2 (10/2014) decoding process analyzer.

i265 implements decoding process as defined by ITU-T H.265 (http://www.itu.int/rec/T-REC-H.265-201504-I/en).

Directory "example" contains analysis files (*.log, *.yuv) from following bitstream:

bitstream:
http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/VPSID_A_VIDYO_2.zip
command line arguments:
./bin/i265 -i VPSID_A_VIDYO_2.bit -n 3 -l all -o 2

author:
Wlodzimierz Lipert https://github.com/vlad83
co-author:
Michal Bartkowiak https://github.com/miszak
