# Mario Kart Wii - Support Package

## Building

You need:

- Python 3
- pyjson5
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
