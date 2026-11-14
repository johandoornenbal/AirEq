#!/usr/bin/env python3
"""Verify vendored hashes and unchanged upstream float/double processing bodies."""
from pathlib import Path
import hashlib
import json
import re

root = Path(__file__).resolve().parents[1]
vendor = root / 'third_party/airwindows'
for name, digest in json.loads((vendor / 'BIQUAD-SHA256.json').read_text()).items():
    assert hashlib.sha256((vendor / name).read_bytes()).hexdigest() == digest, name
for folder in sorted(p for p in vendor.glob('Biquad*') if p.is_dir()):
    name = folder.name
    original = (folder / (name + 'Proc.cpp')).read_text()
    adapted = (root / 'Source' / (name + 'Engine.cpp')).read_text()
    for method in ('processReplacing', 'processDoubleReplacing'):
        def body(source):
            start = source.index('{', source.index('::' + method))
            depth = 0
            for end in range(start, len(source)):
                if source[end] == '{': depth += 1
                elif source[end] == '}': depth -= 1
                if depth == 0: return source[start:end + 1]
            raise AssertionError('Unclosed processing body')
        assert body(original) == body(adapted), name + '::' + method
    print(name + ': unchanged float/double processing bodies')
