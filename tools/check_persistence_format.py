#!/usr/bin/env python3
"""Compare independent native-header and module SDK serialization consumers."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--mcpp', default='mcpp')
parser.add_argument('--toolchain')
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix='opencv-persistence-format-') as directory:
    command = [args.mcpp, 'test', 'persistence', '--features', 'all-modules', '--strict', '--timeout', '120']
    if args.toolchain:
        command += ['--toolchain', args.toolchain]
    env = dict(os.environ, OPENCV_M_PERSISTENCE_OUTPUT=directory)
    subprocess.run(command, cwd=ROOT, env=env, check=True)
    for format in ('yaml', 'xml', 'json', 'yaml-base64', 'xml-base64', 'json-base64'):
        sdk = (Path(directory) / f'sdk-{format}.txt').read_bytes()
        module = (Path(directory) / f'module-{format}.txt').read_bytes()
        assert sdk and sdk == module, f'{format}: module serialization differs from the pinned SDK'
        print(f'{format}: native SDK and module output match ({len(sdk)} bytes)')
