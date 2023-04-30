from abstract_check import Check
from pathlib import Path
from typing import Optional

__all__ = ["PragmaOnceCheck"]

class PragmaOnceCheck(Check):
    def run(self, files: list[Path]) -> Optional[Path]:
        for header in files:
            if header.suffix not in (".hh", ".h"):
                continue

            if "#pragma once" not in header.read_text():
                return header
