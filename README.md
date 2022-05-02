# Mario Kart Wii - Service Pack

Mario Kart Wii - Service Pack is an experimental, open-source, cross-platform mod for MKW aiming to provide a variety of features and improvements over the base game. More details on the [website](https://www.mkw-sp.com).

## Building

You need:

- Python 3 (using PyPy can substantially reduce build times)
- pyjson5 (if installing from pip, the package is `json5` NOT `pyjson5`)
- pyelftools
- ninja (samurai also works)
- devkitPPC (with the DEVKITPPC environment variable set)

Generate the ninja file:

```bash
./configure.py
```

Execute it:

```bash
ninja
```

The `out` directory will contain the generated binaries and assets.

On subsequent builds only the last command needs to be run.

## Contributing

If you are working on something please comment on the relevant issue (or open a new one if necessary).

The codebase uses both C and asm, C should be preferred for full function replacements and for any kind of complex logic. No assumption about the use of registers by C code should be made other than the ABI. If necessary asm wrappers can be employed to restore and backup volatile registers.

There is no automatic formatting yet but the codestyle is pretty consistent so sticking to it shouldn't be too hard.

## Resources

- [Ghidra project](https://drive.google.com/drive/folders/1I1VRfeut3NtPeddePutfAaZhduVdKhhc?usp=sharing) (by far the most complete resource)

- [MKW decompilation](https://github.com/riidefi/mkw) (the most readable if it has what you need)

- [mkw-structures](https://github.com/SeekyCt/mkw-structures) (if you still haven't found)

- [Tockdom Wiki](http://wiki.tockdom.com/wiki/Main_Page) (file format documentation)
