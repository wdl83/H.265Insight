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
    wget -q --directory-prefix=./data --accept=zip --input-file=http://wftp3.itu.int/av-arch/jctvc-site/bitstream_exchange/draft_conformance/HEVC_v1 && \
    find ./data -name '*.zip' -exec unzip -qq -o {} -d ./data/ \; && \
    find ./data -name '*.bit' -exec mv {} ./streams \; && \
    find ./data -name '*.bin' -exec mv {} ./streams \; && \
    sha256sum ./streams/* && \
    virtualenv tests && \
    source ./tests/bin/activate && \
    pip install pytest && \
    py.test ../ci/bitstreams.py -v --capture=sys && \
    deactivate
  fi
}
