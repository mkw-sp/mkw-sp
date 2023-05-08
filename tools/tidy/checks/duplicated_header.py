from abstract_check import Check
from typing import Optional
from pathlib import Path

__all__ = ["DuplicatedHeaderCheck"]

class DuplicatedHeaderCheck(Check):
    def run(self, files: list[Path]) -> Optional[Path]:
        for file in files:
            if file.suffix not in (".h", ".c", ".hh", ".cc"):
                continue

            text = file.read_text()
            if "revolution.h>" in text and file.suffix in (".hh", ".cc"):
                return file

            if "revolution.h" in text and "Common.h" in text:
                return file
