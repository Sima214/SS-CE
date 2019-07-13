#!/usr/bin/env python

import platform
import tarfile
import random
import ftplib
import base64
import time
import sys
import os


build_dir = os.getcwd()

# Get arguments
# First argument is package file - file to be uploaded.
package_file = sys.argv[1]

# Deploy from ci.
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
done = False
while not done:
    try:
        with open(package_file, "rb") as f:
            ftp = ftplib.FTP(server, username, password)
            ftp.storbinary("STOR ssce/%s/%s.tar.bz2" % (os_name, build_number), f)
            ftp.quit()
        done = True
    except Exception as e:
        print(e, flush=True)
        t = random.randint(1, 8)
        print("Trying again in %d seconds..." % (t), flush=True)
        time.sleep(t)