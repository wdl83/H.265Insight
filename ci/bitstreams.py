import os
import sys
import subprocess

stream_path = './streams'
extensions = ('.bit', '.bin')
IGNORE = []
thismodule = sys.modules[__name__]

def get_streams(path):
    for d, _, fs in os.walk(path):
        for f in fs:
            yield os.path.abspath(os.path.join(d, f))

def make_test(fname):
    def proto(*args, **kwds):
        cmd = subprocess.Popen('echo', env=dict(os.environ))
        cmd.communicate()
        test_cmd = 'i265 -i %s'%fname
        cmd = subprocess.Popen(test_cmd.split(), env=dict(os.environ))
        cmd.communicate()
        assert cmd.returncode == 0
    setattr(thismodule, 'test_%s'%str(fname.split('/')[-1].replace('.', '_')), proto)

def make_tests():
    for f in get_streams(stream_path):
        make_test(f)
    for ign in IGNORE:
        if hasattr(thismodule, 'test_%s'%ign):
            delattr(thismodule, 'test_%s'%ign)

make_tests()
