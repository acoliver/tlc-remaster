#!/usr/bin/env python3
"""List object names in an Allegro 4 .dat file"""
import re
import sys

dat_path = sys.argv[1] if len(sys.argv) > 1 else 'bin/data/captaincreation/captaincreation.dat'

with open(dat_path, 'rb') as f:
    data = f.read()

# Search for NAM property markers followed by readable text
names_found = []
for m in re.finditer(b'NAM', data):
    start = m.start()
    end = min(len(data), start + 80)
    chunk = data[start:end]
    # Try to extract readable text after NAM + some control bytes
    for skip in range(3, 12):
        text = ''
        for b in chunk[skip:]:
            if 32 <= b < 127:
                text += chr(b)
            elif b == 0 and len(text) > 2:
                break
            elif len(text) > 0:
                break
        if text and len(text) > 3 and '_' in text:
            names_found.append((start, text))
            break

for offset, name in names_found:
    print(f"Offset {offset}: {name}")
