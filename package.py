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

# Finally output the archive in base64 to the console,
# because I am too bored to actually setup deployments.
with open(output_file, "rb") as f:
    binary = f.read()
    print('-' * 72, flush=True)
    print("Deploying package of %d bytes:" %
          (os.path.getsize(output_file)), flush=True)
    arc64 = base64.b64encode(binary).decode()
    hard_wrap(arc64, 72)
    print('-' * 72, flush=True)
