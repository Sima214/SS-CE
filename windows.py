#!/usr/bin/env python

import subprocess
import argparse
import sys
import os


# Parse command line arguments.
parser = argparse.ArgumentParser(description="Windows build helper")
parser.add_argument('--generator', help='CMake generator')
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

do_call(['cmake', '--version'])

if cmd_args.generator == 'MinGW Makefiles':
    os.environ['PATH'] = '{};{}'.format(
      os.environ['MINGW_PATH'], os.environ['PATH']
    )


def run_build(verbose, test, install):
    os.chdir(cwd)
    # Configure
    print('-' * 72)
    build_dir = '.'
    args = [
      'cmake',
      '-B{}'.format(build_dir)
    ]
    if verbose:
        args += ['-DCMAKE_VERBOSE_MAKEFILE=ON']
    args += ["-DSYSTEM_JEMALLOC=ON"]
    args += ["-DCMAKE_INSTALL_PREFIX:PATH=install"]
    if cmd_args.generator:
        args += ['-G{}'.format(cmd_args.generator)]
    do_call(args)
    # Build
    print('-' * 72)
    args = ['cmake', '--build', build_dir]
    do_call(args)

    # Install
    if install:
        print('-' * 80)
        os.chdir(build_dir)
        args = ['cmake', '--build', build_dir, "--target", "install"]
        do_call(args)
        args = ['cmake', '--build', build_dir, "--target", "package"]
        do_call(args)
        os.chdir(cwd)

    # Test
    if test:
        print('-' * 80)
        os.chdir(build_dir)
        args = ['ctest', '-VV']
        do_call(args)
        os.chdir(cwd)


# Finally run build.
run_build(verbose=True, test=True, install=True)
