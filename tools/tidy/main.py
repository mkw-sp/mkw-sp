import sys
import glob
import subprocess
from pathlib import Path

from checks import checks


import itertools
if __name__ == "__main__":
    extensions = (".py", "h", "c", "hh", "cc")
    check_all = sys.argv[1] == "check_all" if len(sys.argv) > 1 else False

    print("Gathering files...")
    files: list[Path]
    if check_all:
        code_folders = ["payload", "loader", "common", "include", "fuzz", "tools"]

        files = []
        for folder, ext in itertools.product(code_folders, extensions):
            globbed = glob.iglob(f"../../{folder}/**/*.{ext}", recursive=True)
            files.extend(Path(file) for file in globbed)
    else:
        changed_raw = subprocess.check_output(["git", "diff-tree", "--no-commit-id", "--name-only", "HEAD~", "-r"]).decode()
        files = [Path(f"../../{file}") for file in changed_raw.splitlines() if file.endswith(extensions)]

    for i, check in enumerate(checks):
        print(f"Running check {i + 1}/{len(checks)}: {check.description}")
        if (failing_file := check.run(files)) is not None:
            print(f"Failed to run check on {failing_file.name}", file=sys.stderr)
