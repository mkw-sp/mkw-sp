from .duplicated_header import DuplicatedHeaderCheck
from .pragma_once import PragmaOnceCheck

checks = [
    DuplicatedHeaderCheck("Check for revolution.h and Common.h includes"),
    PragmaOnceCheck("Ensure #pragma once is used in headers"),
]
