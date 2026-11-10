#!/usr/bin/env python3
"""Rewrite Shift-JIS C++ sources as UTF-8 in place so that Clang can parse them.

Clang only reads UTF-8. It would also treat a Shift-JIS trail byte of 0x5C at the
end of a // comment as a line continuation and silently lose the next line.
Run this only on a disposable CI checkout, after MSBuild has finished.
Line numbers do not change, so findings still point at the original lines.
"""
import argparse
from pathlib import Path

EXTS = {'.cpp', '.cc', '.cxx', '.h', '.hpp', '.hxx'}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument('dirs', nargs='+', help='Directories to convert recursively')
    args = parser.parse_args()
    converted = 0
    for directory in args.dirs:
        for path in Path(directory).rglob('*'):
            if path.suffix.lower() not in EXTS or not path.is_file():
                continue
            raw = path.read_bytes()
            try:
                raw.decode('utf-8-sig')
            except UnicodeDecodeError:
                path.write_bytes(raw.decode('cp932', errors='replace').encode('utf-8'))
                converted += 1
    print(f'UTF-8 に変換したファイル: {converted} 件')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
