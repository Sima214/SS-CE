#!/usr/bin/env python

import platform
import tarfile
import random
import ftplib
import base64
import time
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
# If third argument, then upload artifact.
upload = len(sys.argv) == 4

# Create tar archive
with tarfile.open(output_file, mode='w:xz') as tar:
    for fn in os.listdir(install_dir):
        os.chdir(install_dir)
        tar.add(fn)
        os.chdir(build_dir)
    for fn in os.listdir(build_dir):
        if os.path.basename(fn).startswith("test_"):
            tar.add(fn)

# Deploy from ci.
if upload:
    print("Deploying package...")
    username = "Sima214"
    password = os.environ["ARTIFACT_PASW"]
    os_name = "unknown"
    platform_name = platform.system()
    if platform_name == "Linux":
        os_name = "linux"
    elif platform_name == "Darwin":
        os_name = "macos"
    elif platform_name == "Windows":
        os_name = "windows"
    else:
        print("Unknown platform: %s" % (platform_name), flush=True)
        sys.exit(1)
    server = "ftp.drivehq.com"
    build_number = os.environ["ARTIFACT_ID"]
    with open(output_file, "rb") as f:
        done = False
        while not done:
            try:
                ftp = ftplib.FTP(server, username, password)
                ftp.storbinary("STOR ssce/%s/%s.tar.xz" % (os_name, build_number), f)
                ftp.quit()
                done = True
            except Exception as e:
                print(e, flush=True)
                t = random.randint(1, 8)
                print("Trying again in %d seconds..." % (t), flush=True)
                time.sleep(t)