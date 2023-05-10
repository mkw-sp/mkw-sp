import glob
import itertools
import sys
from pathlib import Path
from typing import Literal

from checks import checks


def main() -> Literal[-1] | Literal[0]:
    extensions = ("py", "h", "c", "hh", "cc")

    print("Gathering files...")
    files: list[Path]
    if len(sys.argv) == 1:
        code_folders = ["payload", "loader", "common", "include", "fuzz", "tools"]

        files = []
        for folder, ext in itertools.product(code_folders, extensions):
            globbed = glob.iglob(f"{folder}/**/*.{ext}", recursive=True)
            files.extend(Path(file) for file in globbed)
    else:
        files = list(map(Path, sys.argv[1:]))

    failed = False
    for i, check in enumerate(checks):
        print(f"Running check {i + 1}/{len(checks)}: {check.description}")
        if (failing_file := check.run(files)) is not None:
            print(f"Failed to run check on {failing_file.name}", file=sys.stderr)
            failed = True

    return -1 if failed else 0

if __name__ == "__main__":
    sys.exit(main())
