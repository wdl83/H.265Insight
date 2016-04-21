import subprocess
import os
import sys
import requests
from bs4 import BeautifulSoup
import urllib

IGNORE = ['DBLK_A_MAIN10_VIXS_3']
thismodule = sys.modules[__name__]

class make_test:
    def __init__(self): pass
    def __call__(self, u):
        def proto(*args, **kwds):
            url = u
            name = url.split('/')[-1]
            unzip_cmd = 'unzip -o %s'%name

            urllib.urlretrieve (url, name)

            cmd = subprocess.check_output(unzip_cmd.split(), env=dict(os.environ))
            basename = [x.strip().split()[-1] for x in cmd.split('\n') if x.strip().endswith('.bit') or x.strip().endswith('.bin')]
            assert len(basename) > 0

            test_cmd = 'i265 -i %s'%basename[0]

            cmd = subprocess.Popen(test_cmd.split(), env=dict(os.environ))
            cmd.communicate()
            assert cmd.returncode == 0
        setattr(thismodule, 'test_%s'%str(u.split('/')[-1][:-4]), proto)

def make_tests():
    base = 'http://wftp3.itu.int'
    url = base + '/av-arch/jctvc-site/bitstream_exchange/draft_conformance'
    def list_dir(addr, filetype):
        page = requests.get(addr).text
        soup = BeautifulSoup(page, 'html.parser')
        return [str(base + node.get('href')) for node in soup.find_all('a') if node.get('href').endswith(filetype)]
    make = make_test()
    for f in list_dir(url, 'zip'):
        make(f)
    for ign in IGNORE:
        delattr(thismodule, 'test_%s'%ign)

make_tests()
