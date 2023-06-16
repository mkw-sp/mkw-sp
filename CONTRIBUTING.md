# Contributing to MKW-SP

If you are working on something please comment on the relevant issue (or open a new one if necessary).

The codebase uses both C and asm, C should be preferred for full function replacements and for any kind of complex logic. No assumption about the use of registers by C code should be made other than the ABI. If necessary asm wrappers can be employed to restore and backup volatile registers.

The codebase is automatically formatted using `clang-format` (15), this will be checked by CI and must be run before merge.

If you need a unoptimised build with debugging information, use `python3 build.py -- debug`.
