import os
import json
from glob import glob

def SortByValues(d: dict):
    return dict(sorted(d.items(), key=lambda x: x[1].upper()))

def GetFiles():
    files = []
    for ext in ('*.cpp', '*.hpp', '*.c', '*.h'):
        files.extend(glob(os.path.join(os.getcwd(), '**', ext), recursive=True))
    return files

def main():
    seen = {}

    for line in GetFiles():
        line = line.strip()
        line = os.path.relpath(line, os.getcwd())
        base = os.path.basename(line)

        if not seen.get(base):
            seen[base] = [line]
            continue
        else:
            seen[base].append(line)
            print(f"Duplicate found: {base} ({', '.join(seen[base])})")

    out = {k: v for k, v in seen.items() if len(v) > 1}

    print(json.dumps(
        out,
        indent=4
    ))

if __name__ == "__main__":
    main()
