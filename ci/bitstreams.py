import os
import sys
import subprocess
import pytest

stream_path = './streams'
extensions = ('.bit', '.bin')
thismodule = sys.modules[__name__]
IGNORE = [
    'dfab7514dc5803739837a111b14e63013a42210a', # Bitdepth_B_RExt_Sony_1_bin
    '49a4fb424544847172b571aaa3f34cb2e54ad6cc', # GENERAL_12b_444_RExt_Sony_2_bit
    '82bae8f6d87e323a588cf25102cc60a4566e073a', # WAVETILES_RExt_Sony_2_bit
    'a6bc5efd7b495a1898b061a02296668b070ccbdb', # QMATRIX_A_RExt_Sony_1_bit
    'e760329a488e25b1e36cc6c1688b18ab6f1d8dba', # ExplicitRdpcm_B_BBC_2_bit
    '97304210f204aeec5b055fdd592760a373c34b2c', # Bitdepth_A_RExt_Sony_1_bin
    '9da598bff5a39941a57d85bbda502a537abf56eb', # EXTPREC_HIGHTHROUGHPUT_444_16_INTRA_16BIT_RExt_Sony_1_bit
    '6fd86224fb34490c242e43bde035d6a349b4f0f8', # GENERAL_16b_444_RExt_Sony_2_bit
    '71213ef73a64a75651676691fe893d8f3d7b59ee', # GENERAL_16b_400_RExt_Sony_1_bit
    '4ab746eb9c911e56f97db922784020def4e50c1a', # EXTPREC_MAIN_444_16_INTRA_16BIT_RExt_Sony_1_bit
    '947844980938d0d00ab4e658861b6d5217b68ff3', # GENERAL_16b_444_highThroughput_RExt_Sony_2_bit
    'ac9e6784aa7834cfa73734accdae7ae3ce05daa1', # ExplicitRdpcm_A_BBC_1_bit
    '0fc7d8b534412a51d9e515a58e12a3a1be688010', # PERSIST_RPARAM_A_RExt_Sony_3_bit
    '40699703214c1ef374c1e84ae973a9792606b76f', # GENERAL_10b_444_RExt_Sony_2_bit
    'aa3cb828cc50e8156ef9c2446692be51d3cede33', # GENERAL_8b_444_RExt_Sony_2_bit
]

def get_streams(path):
    for d, _, fs in os.walk(path):
        for f in fs:
            yield os.path.abspath(os.path.join(d, f))

def stream_check(stream):
    cmd = subprocess.Popen('echo', env=dict(os.environ))
    cmd.communicate()
    test_cmd = 'i265 -i %s'%stream
    cmd = subprocess.Popen(test_cmd.split(), env=dict(os.environ))
    cmd.communicate()
    assert cmd.returncode == 0

def make_test(fname):
    test_name = fname.split('/')[-1].replace('.', '_')
    if any(filter(lambda x: x in test_name, IGNORE)):
        @pytest.mark.xfail(reason='may fail')
        def proto(*args, **kwds):
            stream_check(fname)
    else:
        def proto(*args, **kwds):
            stream_check(fname)
    setattr(thismodule, 'test_%s'%test_name, proto)

for f in get_streams(stream_path):
    make_test(f)

