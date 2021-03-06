#!/usr/bin/env python

from multiprocessing import cpu_count
import subprocess
import argparse
import shutil
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
os.environ["PATH"] = "%s;%s" % (
    os.path.abspath(cwd), os.environ['PATH']
)


def run_build(verbose, test, trace, package, deploy):
    os.chdir(cwd)
    # Configure
    print('-' * 72, flush=True)
    build_dir = 'build'
    args = [
        'cmake', '-B{}'.format(build_dir)
    ]
    if verbose:
        args += ['-DCMAKE_VERBOSE_MAKEFILE=ON']
    if trace:
        args += ['-DCMAKE_BUILD_TYPE=Debug']
    if cmd_args.generator:
        args += ['-G{}'.format(cmd_args.generator)]
    do_call(args)
    # Build
    print('-' * 72, flush=True)
    args = ['cmake', '--build', build_dir]
    add_parallel(args)
    do_call(args)

    # Install
    if package:
        print('-' * 72, flush=True)
        args = ['cmake', '--build', build_dir, "--target", "package"]
        do_call(args)
        if deploy:
            args = ['cmake', '--build', build_dir, "--target", "deploy"]
            do_call(args)

    # Test
    if test:
        print('-' * 72, flush=True)
        os.chdir(build_dir)
        args = ['ctest', '-VV', '--timeout', '60']
        do_call(args)
        os.chdir(cwd)


# Finally run build.
run_build(verbose=False, trace=False, test=True, package=True, deploy=("APPVEYOR" in os.environ))
