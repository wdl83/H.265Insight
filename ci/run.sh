#!/bin/bash

rm -rf build;
mkdir -p build && \
cd ./build && \
cmake -DABORT_ON_SEI_HASH_MISMATCH:bool=true -DCMAKE_BUILD_TYPE=$BUILD_TYPE ../ && \
make && \
{
  if [[ "${BUILD_TYPE}" == "RELEASE" ]]; then
    export PATH=$PWD/bin:$PATH && \
    mkdir -p ./data && \
    mkdir -p ./streams && \
    mkdir -p ./logs && \
    wget -q --directory-prefix=./data --accept=zip --input-file=http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/RExt && \
    wget -q --directory-prefix=./data --accept=zip --input-file=http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/HEVC_v1 && \
    find ./data -maxdepth 1 -type f -name '*.zip' | while read f; do unzip -qq -o $f -d ./data/`basename $f .zip`; done && \
    find ./data -type f -name '*.bit' -or -name '*.bin' | while read f; do sha1sum $f >> ./logs/streams.sha1 ; mv $f ./streams/`sha1sum $f | awk '{print $1}'`.`basename $f`; done && \
    cat ./logs/streams.sha1 && \
    virtualenv tests && \
    source ./tests/bin/activate && \
    pip install pytest && \
    py.test ../ci/bitstreams.py -v --capture=sys && \
    deactivate
  fi
}
