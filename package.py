#!/usr/bin/env python

import tarfile
import base64
import sys
import os


def hard_wrap(s, l):
    for i in range(0, len(s), l):
        line = s[i:i+l]
        print(line, flush=True)


build_dir = os.getcwd()

# Get arguments
# First is install directory
install_dir = sys.argv[1]
# Second is output destination file
output_file = sys.argv[2] + ".tar.xz"

# Create tar archive
with tarfile.open(output_file, mode='w:xz') as tar:
    for fn in os.listdir(install_dir):
        os.chdir(install_dir)
        tar.add(fn)
        os.chdir(build_dir)
    for fn in os.listdir(build_dir):
        if os.path.basename(fn).startswith("test_"):
            tar.add(fn)
