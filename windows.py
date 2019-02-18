#!/usr/bin/env python3

import argparse
import glob
import os
import platform
import shutil
import subprocess
import sys

parser = argparse.ArgumentParser(description="Windows build script")
parser.add_argument(
    '--install-dir', help='Custom install directory'
)
parser.add_argument(
    '--generator', help='CMake generator'
)
cmd_args = parser.parse_args()

cwd = os.getcwd()

def do_call(args):
  oneline = ''
  for i in args:
    oneline += ' "{}"'.format(i)
  print('[{}]>{}'.format(os.getcwd(), oneline))
  try:
    subprocess.check_call(args, env=os.environ)
  except subprocess.CalledProcessError as error:
    print(error)
    print(error.output)
    sys.exit(1)

if os.name == 'nt':
  do_call(['where', 'cmake'])
else:
  do_call(['which', 'cmake'])

do_call(['cmake', '--version'])

if cmd_args.install_dir:
  install_dir = cmd_args.install_dir
else:
  install_dir = os.path.join(cwd, 'install')

exe_dir = os.path.join(install_dir, 'bin')

if platform.system() == 'Windows' or platform.system().startswith('CYGWIN'):
  # Update PATH for DLL platforms
  windows_path_update = True
else:
  windows_path_update = False

if windows_path_update:
  os.environ['PATH'] = '{};{}'.format(exe_dir, os.environ['PATH'])

if cmd_args.generator == 'MinGW Makefiles':
  os.environ['PATH'] = '{};{}'.format(
      os.environ['MINGW_PATH'], os.environ['PATH']
  )

def run_build(verbose, test):
  os.chdir(cwd)
  # Configure
  print('-' * 80)
  build_dir = '.'
  args = [
      'cmake',
      '-B{}'.format(build_dir)
  ]
  if verbose:
    args += ['-DCMAKE_VERBOSE_MAKEFILE=ON']
  args += ["-DSYSTEM_JEMALLOC=ON"]
  if cmd_args.generator:
    args += ['-G{}'.format(cmd_args.generator)]
  do_call(args)
  # Build
  print('-' * 80)
  args = ['cmake', '--build', build_dir]
  do_call(args)

  # Test
  if test:
    print('-' * 80)
    os.chdir(build_dir)
    args = ['ctest', '-VV']
    do_call(args)
    os.chdir(cwd)

# Finally run build.
run_build(verbose=True, test=True)