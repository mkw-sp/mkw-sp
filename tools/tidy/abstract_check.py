from abc import ABC, abstractmethod
from pathlib import Path
from typing import Optional

class Check(ABC):
    def __init__(self, description: str) -> None:
        self.description = description

    # Runs the check on the given files.
    # If the check fails, this method returns the Path that failed.
    @abstractmethod
    def run(self, files: list[Path]) -> Optional[Path]: ...
