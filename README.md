# Mario Kart Wii - Service Pack

Mario Kart Wii - Service Pack is an experimental, open-source, cross-platform mod for MKW aiming to provide a variety of features and improvements over the base game. More details on the [website](https://mkw-sp.com).

## Building

You need:

- devkitPPC (with the DEVKITPPC environment variable set)
- ninja (samurai also works)
- protoc
- Python 3
- pyjson5 (if installing from pip, the package is `json5` NOT `pyjson5`)
- pyelftools
- itanium\_demangler
- protobuf (the Python package)

Generate the ninja file:

```bash
./configure.py
```

Execute it:

```bash
ninja
```

The `out` directory will contain the generated binaries and assets.

The `configure.py` command only needs to be run when a file has been added or removed
to the project, and can be skipped if only recompiling with changed files.

## Contributing

If you are working on something please comment on the relevant issue (or open a new one if necessary).

The codebase uses both C and asm, C should be preferred for full function replacements and for any kind of complex logic. No assumption about the use of registers by C code should be made other than the ABI. If necessary asm wrappers can be employed to restore and backup volatile registers.

The codebase is automatically formatted using `clang-format` (15), this will be checked by CI and must be run before merge.

If you need a unoptimised build with debugging information, use `ninja debug` to build once, or use
`configure.py --default-targets` and then a comma seperated list of `test`, `debug`, or `release` to
automatically build the requested binaries with every `ninja` invoke.

## Resources

- [Ghidra project](https://drive.google.com/drive/folders/1I1VRfeut3NtPeddePutfAaZhduVdKhhc?usp=sharing) (by far the most complete resource)

- [MKW decompilation](https://github.com/riidefi/mkw) (the most readable if it has what you need)

- [mkw-structures](https://github.com/SeekyCt/mkw-structures) (if you still haven't found)

- [Tockdom Wiki](http://wiki.tockdom.com/wiki/Main_Page) (file format documentation)
