#!/usr/bin/env python

from multiprocessing import cpu_count
import subprocess
import argparse
import sys
import os


# Parse command line arguments.
parser = argparse.ArgumentParser(description="Windows build helper")
parser.add_argument('--generator', help='CMake generator')
cmd_args = parser.parse_args()

cwd = os.getcwd()


def add_parallel(args):
    args += ['--parallel', str(cpu_count())]


def do_call(args):
    oneline = ''
    for i in args:
        oneline += ' "{}"'.format(i)
    print('[{}]>{}'.format(os.getcwd(), oneline), flush=True)
    try:
        subprocess.check_call(args, env=os.environ)
    except subprocess.CalledProcessError as error:
        print(error, flush=True)
        print(error.output, flush=True)
        sys.exit(1)


do_call(['cmake', '--version'])

if cmd_args.generator == 'MinGW Makefiles':
    os.environ['PATH'] = '{};{}'.format(
        os.environ['MINGW_PATH'], os.environ['PATH']
    )


def run_build(verbose, test, trace, install):
    os.chdir(cwd)
    # Configure
    print('-' * 72, flush=True)
    build_dir = '.'
    args = [
        'cmake', '-B{}'.format(build_dir)
    ]
    if verbose:
        args += ['-DCMAKE_VERBOSE_MAKEFILE=ON']
    if trace:
        args += ['-DCMAKE_BUILD_TYPE=Debug']
    args += ["-DSYSTEM_JEMALLOC=ON"]
    args += ["-DCMAKE_INSTALL_PREFIX:PATH=install"]
    if cmd_args.generator:
        args += ['-G{}'.format(cmd_args.generator)]
    do_call(args)
    # Build
    print('-' * 72, flush=True)
    args = ['cmake', '--build', build_dir]
    add_parallel(args)
    do_call(args)

    # Install
    if install:
        print('-' * 72, flush=True)
        os.chdir(build_dir)
        args = ['cmake', '--build', build_dir, "--target", "install"]
        do_call(args)
        args = ['cmake', '--build', build_dir, "--target", "package"]
        do_call(args)
        os.chdir(cwd)

    # Test
    if test:
        print('-' * 72, flush=True)
        os.chdir(build_dir)
        args = ['ctest', '-VV']
        do_call(args)
        os.chdir(cwd)


# Finally run build.
run_build(verbose=False, trace=True, test=True, install=True)
