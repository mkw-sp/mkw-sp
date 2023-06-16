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

Compile the project by running `build.py`:
```bash
./build.py
```

The `out` directory will contain the generated binaries and assets.

## Contributing

[CONTRIBUTING.md](CONTRIBUTING.md)

## Resources

- [Ghidra project](https://drive.google.com/drive/folders/1I1VRfeut3NtPeddePutfAaZhduVdKhhc?usp=sharing) (by far the most complete resource)

- [MKW decompilation](https://github.com/riidefi/mkw) (the most readable if it has what you need)

- [mkw-structures](https://github.com/SeekyCt/mkw-structures) (if you still haven't found)

- [Tockdom Wiki](http://wiki.tockdom.com/wiki/Main_Page) (file format documentation)
